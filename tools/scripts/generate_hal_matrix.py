#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import lbuild, re, functools, sys, subprocess
from collections import defaultdict
from jinja2 import Environment
from pathlib import Path

repopath = lambda path: Path(__file__).parents[2] / path

def hal_get_modules():
    builder = lbuild.api.Builder()
    builder._load_repositories(repopath("repo.lb"))

    # Get me a condensed list of targets
    target_option = builder.parser.find_option(":target")
    minimal_targets = defaultdict(list)

    for target in target_option.values:
        target_option.value = target
        target = target_option.value._identifier
        short_id = target.copy()

        if target.platform == "avr":
            short_id.naming_schema = short_id.naming_schema \
                .replace("{type}-{speed}{package}", "") \
                .replace("-{speed}{package}", "")

        elif target.platform == "stm32":
            short_id.naming_schema = "{platform}{family}{name}"

        elif target.platform == "hosted":
            short_id.naming_schema = "{platform}-{family}"

        elif target.platform == "sam":
            short_id.naming_schema = "{platform}{family}{series}"

        short_id.set("platform", target.platform) # invalidate caches
        minimal_targets[short_id.string].append(target)

    targets = []
    # Sort the targets by name in their category
    # And choose the last one (assumed to be the "largest" devices)
    for key, values in minimal_targets.items():
        targets.append(sorted(values, key=lambda d: d.string)[-1])
    # print(targets)

    # Prime the repositories and get all module files
    mfiles = []
    target_option.value = targets[0].string
    for repo in builder.parser._findall(builder.parser.Type.REPOSITORY):
        repo.prepare()
        mfiles.extend([(repo, file) for file in repo._module_files])

    print("Querying for {} targets...".format(len(targets)))

    # targets = [t for t in targets if t.platform == "avr"]

    all_targets = {}
    file_cache = {}
    mapping = defaultdict(set)
    for target in targets:
        target_option.value = target.string
        device = target_option.value
        device.get_driver("core")
        drivers = set(d["name"] for d in device._properties["driver"])
        # print(sorted(list(drivers)))

        modules = set()
        # We only care about some modm:platform:* modules here
        not_interested = {"bitbang", "common", "heap", "clock", "core", "fault", "cortex-m", "uart.spi", "itm", "rtt"}
        imodules = (m  for (repo, mfile) in mfiles  for m in lbuild.module.load_module_from_file(repo, mfile)
                    if m.available and m.fullname.startswith("modm:platform:") and
                    all(p not in m.fullname for p in not_interested))
        for module in imodules:
            # lookup the mapping of a module onto the hardware names
            if module.filename not in file_cache:
                deps = re.findall(r'\.(has_driver|get_driver|get_all_drivers)\("(.*?)"\)',
                                     Path(module.filename).read_text())
                deps = set(m[1].split(":")[0] for m in deps)
                deps -= {'core'} # Remove false positives
                file_cache[module.filename] = deps
            mname = module.fullname.replace("modm:platform:", "").split(":")[0]
            # Remap naming to be common between devices and better understood
            remap = {
                "comp": "Comparator",
                "eth": "Ethernet",
                "random": "Random Generator",
                "id": "Unique ID",
                "rcc": "System Clock",
                "gclk": "System Clock",
                "extint": "External Interrupt",
                "fsmc": "External Memory",
                "flash": "Internal Flash",
                "timer": "Timer",
                "i2c": "I<sup>2</sup>C",
            }
            mname = remap.get(mname, mname.upper())
            modules.add(mname)
            mapping[mname].update(file_cache[module.filename])

        # External interrupt is currently part of the GPIO module
        if target.platform in ["avr", "stm32"] and "GPIO" in modules:
            modules.add("External Interrupt")

        # print(target, dict(modules))
        print(target, end=" ", flush=True)
        all_targets[target] = (drivers, modules)

    # Some information cannot be extracted from the module.lb files
    mapping["UART"].update({"usi", "sercom"})
    mapping["Timer"].update({"tc", "tcc"})
    mapping["SPI"].add("sercom")
    mapping["I<sup>2</sup>C"].add("sercom")
    mapping["USB"].add("usb")
    mapping["CAN"].add("fdcan")
    mapping["DMA"].add("dmac")
    mapping["Comparator"].add("ac")
    mapping["Internal Flash"].add("nvmctrl")
    mapping["External Memory"].add("quadspi")

    print(); print()
    return (all_targets, mapping)

def hal_module_filter(targets, family):
    family = family.replace("d21", "d")
    drivers = [v[0] for k,v in targets[0].items() if k.family == family]
    modules = [v[1] for k,v in targets[0].items() if k.family == family]

    all_peripherals = functools.reduce(lambda l, r: l.union(r), drivers, set())
    implemented_peripherals = functools.reduce(lambda l, r: l.union(r), modules, set())

    return (all_peripherals, implemented_peripherals)


def hal_create_table(targets, platforms, common_table=False):
    families = set((t.platform.replace("avr", "at"), t.family.replace("d", "d21"))
                   for t in targets[0] if any(t.platform == p for p in platforms))
    if not families: return (None, None) if common_table else None;
    modules = {f:hal_module_filter(targets, f[1]) for f in families}
    drivers = {f:v[1] for f, v in modules.items()}
    all_drivers = functools.reduce(lambda l, r: l.union(r), drivers.values())
    common_drivers = functools.reduce(lambda l, r: l.intersection(r), drivers.values())
    if common_table:
        all_drivers -= common_drivers
        common_pre = list(set(f[0] for f in families))[0]
    mapping = targets[1]

    print(platforms, dict(mapping))
    values = defaultdict(lambda: defaultdict(lambda: "✗"))
    for driver in all_drivers:
        for f, v in modules.items():
            # mapping from driver to peripheral
            if driver in v[1]:
                values[f][driver] = "✅"
            elif any(p in v[0] for p in mapping[driver]):
                values[f][driver] = "○"

    colspan = [(p, len([fp for fp in families if fp[0] == p]))
                for p in set(f[0] for f in families)]
    colspan = list(reversed(sorted(colspan)))
    print(families, colspan)
    families = sorted(list(families),
                      key=lambda f: ([c[0] for c in colspan].index(f[0]), f[1]))
    subs = {"pers": all_drivers, "families": families,
            "values": values, "colspan": colspan}

    TABLE_HTML = r"""
<table>
<tr>
<th align="center"></th>
{% for pre, span in colspan  -%}
<th align="center" colspan="{{ span }}">{{ pre | upper }}</th>
{% endfor %}</tr><tr>
<th align="left">Peripheral</th>
{% for fam in families  -%}
<th align="center">{{ fam[1] | capitalize }}</th>
{% endfor %}
{%- for per in pers | sort %}</tr><tr>
<td align="left">{{ per }}</td>
    {%- for fam in families %}
<td align="center">{{ values[fam][per] }}</td>
    {%- endfor %}
{% endfor %}</tr>
</table>

"""
    return Environment().from_string(TABLE_HTML).render(subs)

def hal_format_tables():
    tables = {}
    targets = hal_get_modules()

    # tables["avr"] = hal_create_table(targets, ["avr"])
    # tables["stm32"] = hal_create_table(targets, ["stm32"])
    # tables["sam"] = hal_create_table(targets, ["sam"])

    tables["all"] = hal_create_table(targets, ["avr", "stm32", "sam"])

    return tables


hal_tables = hal_format_tables()
readme = repopath("README.md")
readme.write_text(re.sub(
        r"<!--alltable-->.*?<!--/alltable-->",
         "<!--alltable-->{}<!--/alltable-->".format(hal_tables["all"]),
        readme.read_text(), flags=re.DOTALL | re.MULTILINE))

# Check git differences and fail
if "-d" in sys.argv:
    differences = subprocess.run("git diff", shell=True, cwd=repopath("."),
            stdout=subprocess.PIPE).stdout.decode("utf-8").strip(" \n")
    if len(differences):
        subprocess.run("git --no-pager diff", shell=True, cwd=repopath("."))
        print("\nPlease synchronize the HAL implementation table:\n\n"
              "    $ python3 tools/scripts/generate_hal_matrix.py\n\n"
              "and then commit the results!")
        exit(1)

exit(0)
