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

import re, os, sys, subprocess, lbuild
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

def run(where, command, stdin=None):
  print(command)
  result = subprocess.run(command, shell=True, cwd=where, input=stdin, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  return (result.returncode,
          result.stdout.decode("utf-8").strip(" \n"),
          result.stderr.decode("utf-8").strip(" \n"))

def name(raw_name):
    result = []
    raw_name = raw_name.replace("_", " ").replace(".", " ").replace(":", " ")
    for part in raw_name.split(" "):
        part = part.upper()
        result.append(part)

    result = "-".join(result)
    # Rename some things to be more accurate
    result = result.replace("BLUE-PILL", "Blue Pill")\
                   .replace("BLACK-PILL", "Black Pill")\
                   .replace("ARDUINO-UNO", "Arduino UNO")\
                   .replace("ARDUINO-NANO", "Arduino NANO")\
                   .replace("GENERIC", "Generic")\
                   .replace("LINUX", "Linux")\
                   .replace("WINDOWS", "Windows")\
                   .replace("HMC58X", "HMC58x")\
                   .replace("HCLAX", "HCLAx")\
                   .replace("PARALLEL-", "")\
                   .replace("BLOCK-DEVICE-", "")\
                   .replace("BLOCK-", "")\
                   .replace("-SPI", "")
    if result in ["DEVICE", "LIS3-TRANSPORT", "MEMORY-BUS", "TERMINAL",
                  "MIRROR", "ADC-SAMPLER", "FAT", "HEAP", "--PYCACHE--"]:
        return None
    return result

def url(path):
    # this should point to the module API documentation instead of the source code
    return "https://github.com/modm-io/modm/tree/develop/" + str(path)

def replace(text, key, content):
    return re.sub(r"<!--{0}-->.*?<!--/{0}-->".format(key), "<!--{0}-->{1}<!--/{0}-->".format(key, content), text, flags=re.DOTALL | re.MULTILINE)

def extract(text, key):
    return re.search(r"<!--{0}-->(.*?)<!--/{0}-->".format(key), text, flags=re.DOTALL | re.MULTILINE).group(1)

def format_table(items, width, align=None):
    subs = {"items": items, "width": width}
    if align: subs["align"] = align;
    return Environment().from_string(TABLE_TEMPLATE).render(subs)

def get_lbuild_in(path):
    builder = lbuild.api.Builder(cwd=path, config=join(path, "project.xml"))
    builder.load()
    return builder.parser


# All the paths
root = repopath(".")
board_path = root / "src/modm/board"
example_path = root / "examples"
ignored_path = root / "test/all/ignored.txt"
author_path = root / "AUTHORS"

examples_readme_path = root / "examples/README.md"
readme_path = root / "README.md"
getting_started_in_path = root / "docs/getting-started.md.in"
getting_started_path = root / "docs/src/guide/getting-started.md"
index_in_path = root / "docs/index.md.in"
index_path = root / "docs/src/index.md"
whoweare_in_path = root / "docs/who-we-are.md.in"
whoweare_path = root / "docs/src/who-we-are.md"
modules_in_path = root / "docs/modules.md.in"
modules_path = root / "docs/src/reference/modules.md"

# We cannot use lbuild to enumerate the boards since they only make themselves available for certain devices
# boards = [{"name": name(x.name), "url": url(x.relative_to(root))} for x in board_path.iterdir() if x.is_dir() and name(x.name)]
boards = [{"name": name(x.name), "url": None} for x in board_path.iterdir() if x.is_dir() and name(x.name)]
boards.sort(key=lambda b: b["name"])
bsp_table = format_table(boards, 4)

# Get all the example directory paths
examples = [e.relative_to(example_path) for e in example_path.glob('**/project.xml')]
examples = [{"name": "{}: {}".format(name(str(e.parts[0])), e.parent.relative_to(e.parts[0])), "url": url(Path("examples") / e.parent / "main.cpp")} for e in examples]
examples.sort(key=lambda b: b["name"])
example_table = format_table(examples, 2, "left")

# Get all supported targets
lbuild_parser = get_lbuild_in("{}/examples/stm32f4_discovery/blink/".format(root))
targets = set(lbuild_parser.find_option("modm:target").values)
ignored_devices = set(d for d in ignored_path.read_text().strip().splitlines() if "#" not in d)
targets -= ignored_devices
avr_count = len([t for t in targets if t.startswith("at")])
stm_count = len([t for t in targets if t.startswith("stm32")])

# get the author count
author_count = len(author_path.read_text().strip().splitlines())

# Get all the modules that are available for the STM32
# Get all drivers, we assume they are available for all devices
drivers = sorted([m.replace("modm:driver:", "") for m in lbuild_parser.modules if m.startswith("modm:driver:")])
drivers = [{"name": name(d), "url": None} for d in drivers if name(d)]
driver_table = format_table(drivers, 6)

# Read the repo README.md and replace these keys
readme = readme_path.read_text()
readme = replace(readme, "authorcount", author_count - 7)
readme = replace(readme, "avrcount", avr_count)
readme = replace(readme, "stmcount", stm_count)
readme = replace(readme, "bsptable", bsp_table)
readme = replace(readme, "drivertable", driver_table)
readme_path.write_text(readme)

# extract these keys
links = extract(readme, "links")
authors = extract(readme, "authors")
# remove
readme = re.sub(r"((<!--webignore-->.*?<!--/webignore-->)|(<!--links-->.*?<!--/links-->)|(<!--/?bsptable-->))\n", "", readme, flags=re.DOTALL | re.MULTILINE)

index = Environment().from_string(index_in_path.read_text()).render({"content": readme, "links": links, "example_table": example_table})
index_path.write_text(index)

whoweare = Environment().from_string(whoweare_in_path.read_text()).render({"authors": authors, "links": links})
whoweare_path.write_text(whoweare)

# Read the examples README.md and replace these keys
readme = examples_readme_path.read_text()
# readme = replace(readme, "examplecount", len(examples))
# examples_readme_path.write_text(readme)

getting_started = Environment().from_string(getting_started_in_path.read_text()).render({"examples": extract(readme, "examples")})
getting_started_path.write_text(getting_started)

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
