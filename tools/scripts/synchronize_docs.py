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

import re, os, sys, subprocess, lbuild, shutil
from pathlib import Path
from jinja2 import Environment
from os import listdir
from os.path import isfile, join, abspath

TABLE_TEMPLATE = \
r"""
<table>
<tr>
{%- for item in items %}
<td align="{% if align is defined %}{{align}}{% else %}center{% endif %}">{% if item.url %}<a href="{{item.url}}">{% endif %}{{item.name}}{% if item.url %}</a>{% endif %}</td>{%- if loop.index % width == 0 %}
</tr><tr>{%- endif -%}
{%- endfor %}
</tr>
</table>

"""

def repopath(path):
    return (Path(os.path.abspath(__file__)).parents[2] / path)

with open(repopath("tools/scripts/generate_hal_matrix.py")) as hal_tables:
    exec(hal_tables.read())

def run(where, command, stdin=None):
    print(command)
    result = subprocess.run(command, shell=True, cwd=where, input=stdin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return (result.returncode,
            result.stdout.decode("utf-8").strip(" \n"),
            result.stderr.decode("utf-8").strip(" \n"))

def name(raw_name):
    result = []
    raw_name = str(raw_name).replace("_", " ").replace(".", " ").replace(":", " ")
    for part in raw_name.split(" "):
        part = part.upper()
        result.append(part)

    result = "-".join(result)
    # Rename some things to be more accurate
    result = result.replace("BLUE-PILL-", "Blue Pill ")\
                   .replace("BLACK-PILL-", "Black Pill ")\
                   .replace("ARDUINO-UNO", "Arduino UNO")\
                   .replace("ARDUINO-NANO", "Arduino NANO")\
                   .replace("RASPBERRYPI", "Raspberry Pi")\
                   .replace("GENERIC", "Generic")\
                   .replace("LINUX", "Linux")\
                   .replace("WINDOWS", "Windows")\
                   .replace("HMC58X", "HMC58x")\
                   .replace("HCLAX", "HCLAx")\
                   .replace("PARALLEL-", "")\
                   .replace("BLOCK-DEVICE-", "")\
                   .replace("BLOCK-", "")\
                   .replace("-SPI", "")
    if result in ["DEVICE", "LIS3-TRANSPORT", "MEMORY-BUS", "TERMINAL", "ALLOCATOR",
                  "MIRROR", "ADC-SAMPLER", "FAT", "HEAP", "--PYCACHE--", "FILE"]:
        return None
    return result

def fmt_url(name):
    return str(name).replace(".", "-").replace(":", "-")

def github_url(path):
    return "https://github.com/modm-io/modm/tree/develop/" + str(path)

def board_url(name):
    return "https://modm.io/reference/module/modm-board-" + fmt_url(name)

def driver_url(name):
    return "https://modm.io/reference/module/modm-driver-" + fmt_url(name)

def replace(text, key, content):
    return re.sub(r"<!--{0}-->.*?<!--/{0}-->".format(key), "<!--{0}-->{1}<!--/{0}-->".format(key, content), text, flags=re.DOTALL | re.MULTILINE)

def extract(text, key):
    return re.search(r"<!--{0}-->(.*?)<!--/{0}-->".format(key), text, flags=re.DOTALL | re.MULTILINE).group(1)

def format_table(items, width, align=None):
    subs = {"items": items, "width": width}
    if align: subs["align"] = align;
    return Environment().from_string(TABLE_TEMPLATE).render(subs)

def get_lbuild(root, target=None):
    if target is None: target = "stm32";
    target = {"avr": "atmega2560-16au", "stm32": "stm32f469nih6",
              "hosted": "hosted-linux"}[target]
    builder = lbuild.api.Builder(cwd=root, options=[":target={}".format(target)])
    builder.load(root / "repo.lb")
    return builder.parser


# All the paths
root = repopath(".")
board_path = root / "src/modm/board"
example_path = root / "examples"
ignored_path = root / "test/all/ignored.txt"
examples_readme_in_path = root / "examples/README.md"
examples_readme_path = root / "docs/src/guide/examples.md"
readme_path = root / "README.md"
index_in_path = root / "docs/index.md.in"
index_path = root / "docs/src/index.md"
whoweare_in_path = root / "docs/who-we-are.md.in"
whoweare_path = root / "docs/src/who-we-are.md"
modules_in_path = root / "docs/modules.md.in"
modules_path = root / "docs/src/reference/modules.md"

# We cannot use lbuild to enumerate the boards since they only make themselves available for certain devices
boards = [re.search(r"<module>modm:board:(.*?)</module>", config.read_text()).group(1)
          for config in Path(repopath("src/modm/board")).glob("*/board.xml")]
boards = [{"name": name(b), "url": board_url(b)} for b in boards]
boards.sort(key=lambda b: b["name"])
bsp_table = format_table(boards, 4)

# Get all the example directory paths
examples = [e.relative_to(example_path) for e in example_path.glob('**/project.xml')]
examples = [{"name": "{}: {}".format(name(str(e.parts[0])), e.parent.relative_to(e.parts[0])),
             "url": github_url(Path("examples") / e.parent / "main.cpp")} for e in examples]
examples.sort(key=lambda b: b["name"])
example_table = format_table(examples, 2, "left")

# Get all supported targets
targets = set(get_lbuild(root).find_option("modm:target").values)
ignored_devices = set(d for d in ignored_path.read_text().strip().splitlines() if "#" not in d)
targets -= ignored_devices
avr_count = len([t for t in targets if t.startswith("at")])
stm_count = len([t for t in targets if t.startswith("stm32")])
sam_count = len([t for t in targets if t.startswith("sam")])
all_count = avr_count + stm_count + sam_count

# get the author count
from authors import author_handles
author_count = len(author_handles)

# Get all the modules that are available for the STM32
# Get all drivers, we assume they are available for all devices
drivers = (get_lbuild(root, t).modules for t in {"avr", "stm32", "hosted"})
drivers = {m for mg in drivers for m in mg if m.startswith("modm:driver:")}
drivers = sorted(m.replace("modm:driver:", "") for m in drivers)
drivers = [{"name": name(d), "url": driver_url(d)} for d in drivers if name(d)]
driver_table = format_table(drivers, 6)

# Read the repo README.md and replace these keys
hal_tables = hal_format_tables()
readme = readme_path.read_text()
readme = replace(readme, "authorcount", author_count - 7)
readme = replace(readme, "avrcount", avr_count)
readme = replace(readme, "samcount", sam_count)
readme = replace(readme, "stmcount", stm_count)
readme = replace(readme, "allcount", all_count)
readme = replace(readme, "bsptable", bsp_table)
readme = replace(readme, "alltable", hal_tables["all"])
readme = replace(readme, "drivertable", driver_table)
readme_path.write_text(readme)

# extract these keys
links = extract(readme, "links")
authors = extract(readme, "authors").replace("\\@", "@")
# remove
readme = re.sub(r"((<!--webignore-->.*?<!--/webignore-->)|(<!--links-->.*?<!--/links-->)|(<!--/?bsptable-->))\n", "", readme, flags=re.DOTALL | re.MULTILINE)

index = Environment().from_string(index_in_path.read_text()).render({"content": readme, "links": links, "example_table": example_table})
index_path.write_text(index)

whoweare = Environment().from_string(whoweare_in_path.read_text()).render({"authors": authors, "links": links})
whoweare_path.write_text(whoweare)

# Copy the example readme over
shutil.copy(examples_readme_in_path, examples_readme_path)

# Check git differences and fail
if "-d" in sys.argv:
    differences = run(repopath("."), r"git diff")[1]
    if len(differences):
        subprocess.run("git --no-pager diff", shell=True, cwd=repopath("."))
        print("\nPlease synchronize the modm documentation:\n\n"
              "    $ python3 tools/scripts/synchronize_docs.py\n\n"
              "and then commit the results!")
        exit(1)

exit(0)
