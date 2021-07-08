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
from collections import defaultdict

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

repopath = lambda path: Path(__file__).parents[2] / path

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
    builder.load(repopath("repo.lb"))
    return builder.parser


# All the paths
board_path = repopath("src/modm/board")
example_path = repopath("examples")
examples_readme_in_path = repopath("examples/README.md")
examples_readme_path = repopath("docs/src/guide/examples.md")
readme_path = repopath("README.md")
index_in_path = repopath("docs/index.md.in")
index_path = repopath("docs/src/index.md")
whoweare_in_path = repopath("docs/who-we-are.md.in")
whoweare_path = repopath("docs/src/who-we-are.md")
modules_in_path = repopath("docs/modules.md.in")
modules_path = repopath("docs/src/reference/modules.md")
changelog_in_paths = repopath("docs/release").glob("20*.md")
changelog_path = repopath("CHANGELOG.md")

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
targets = set(get_lbuild(repopath(".")).find_option("modm:target").values)
avr_count = len([t for t in targets if t.startswith("at")])
stm_count = len([t for t in targets if t.startswith("stm32")])
sam_count = len([t for t in targets if t.startswith("sam")])
all_count = avr_count + stm_count + sam_count

# Get all the modules that are available for the STM32
# Get all drivers, we assume they are available for all devices
drivers = (get_lbuild(repopath("."), t).modules for t in {"avr", "stm32", "hosted"})
drivers = {m for mg in drivers for m in mg if m.startswith("modm:driver:")}
drivers = sorted(m.replace("modm:driver:", "") for m in drivers)
drivers = [{"name": name(d), "url": driver_url(d)} for d in drivers if name(d)]
driver_table = format_table(drivers, 6)

# Read the repo README.md and replace these keys
readme = readme_path.read_text()
readme = replace(readme, "avrcount", avr_count)
readme = replace(readme, "samcount", sam_count)
readme = replace(readme, "stmcount", stm_count)
readme = replace(readme, "allcount", all_count)
readme = replace(readme, "bsptable", bsp_table)
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

# Compose changelog
crlinks = set()
cchangelog = "\n\n"
for path in reversed(sorted(changelog_in_paths)):
    content = path.read_text()
    content = re.sub(r"as (`modm:[^:][^ ]+?`)\.\n", r"as [\1][].\n", content)
    content = re.sub(r"(#\d+)", r"[\1][]", content)
    content = re.sub(r"(@[\w\d\-_]+)", r"[\1][]", content)
    content = re.sub(r"-> ([\da-f]{7})", r"-> [\1][]", content)
    content = re.sub(r"(20[12]\dq[1-4])\.\n", r"[\1][].\n", content)
    cchangelog += content + "\n\n"
    crlinks.update(re.findall(r"\[(.*?)\]\[\]", content))
# Generate appropriate link
clinks = defaultdict(dict)
for link in crlinks:
    if link.startswith("@"): # user handle
        clinks["@"][link] = "https://github.com/" + link[1:]
    elif link.startswith("#"): # PR or Issue
        clinks["#"][link] = "https://github.com/modm-io/modm/pull/" + link[1:]
    elif link.startswith("`"): # lbuild module
        clinks["`"][link] = "https://modm.io/reference/module/" + \
                link[1:-1].replace(".", "-").replace(":", "-")
    elif link.startswith("20") and len(link) == 6: # Release tag
        clinks["r"][link] = "https://github.com/modm-io/modm/releases/tag/" + link
    elif len(link) == 7: # Commit
        clinks["c"][link] = "https://github.com/modm-io/modm/commit/" + link
    else:
        print("Unknown changelog link:", link)
# Format the links
links = ""
links += "\n\n" + "\n".join(sorted("[{}]: {}".format(name, url) for name, url in clinks["r"].items()))
links += "\n\n" + "\n".join(sorted("[{}]: {}".format(name, url) for name, url in clinks["@"].items()))
links += "\n\n" + "\n".join(sorted("[{}]: {}".format(name, url) for name, url in clinks["`"].items()))
links += "\n\n" + "\n".join(sorted("[{}]: {}".format(name, url) for name, url in clinks["#"].items()))
links += "\n\n" + "\n".join(sorted("[{}]: {}".format(name, url) for name, url in clinks["c"].items()))
# Format the changelog file
changelog = changelog_path.read_text()
changelog = replace(changelog, "releases", cchangelog)
changelog = replace(changelog, "links", links + "\n\n")
changelog_path.write_text(changelog)

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
