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

from collections import defaultdict
import subprocess
import sys
import re
import os

path_mapping = [
    (r"src/xpcc/", r"src/modm/"),
    (r"src/modm/architecture/platform/driver/(.+?)/generic/", r"src/modm/platform/\1/common/"),
    (r"src/modm/architecture/platform/driver/", r"src/modm/platform/"),
    (r"src/modm/architecture/platform/board/", r"src/modm/board/"),
    (r"src/modm/architecture/platform/devices/", None),
    (r"examples/lpcxpresso/", None),
    (r"examples/stm32/", None),
    (r"examples/stm32f4_loa.*?/", None),
    (r"examples/.*?/SConstruct", None),
    (r"src/(.*?)/test/(.*)", r"test/\1/\2"),
    (r"tools/bootloader/", None),
    (r"ext/", None),
    (r"tools/device_files/", None),
    (r"tools/device_file_generator/", None),
    (r".*?/build\.cfg", None),
    (r"src/unittest_.*?\.cfg", None),
    (r"templates/cmake/", None),
    (r"scons/", None),
    (r".*?/project.cfg", None),
    (r"README\.md", None),
    (r"PORTING\.md", None),
    (r"CHANGELOG\.md", r"docs/CHANGELOG.md"),
    (r".travis.*?", None),
    (r".circleci/", None),
]

diff_mapping = [
    (r"xpcc", r"modm"),
    (r"XPCC", r"MODM"),
    (r"(\w)__(\w)", r"\1_\2"),
    (r"modm::SoftwareGpioPort", r"SoftwareGpioPort"),
    (r"modm::GpioUnused", "GpioUnused"),
    (r"modm::stm32", "modm::platform"),
    (r"modm::atmega", "modm::platform"),
    (r"modm::attiny", "modm::platform"),
    (r"modm::avr", "modm::platform"),
    (r"modm::at90", "modm::platform"),
    (r"/roboterclubaachen/modm/", "/roboterclubaachen/xpcc/"),
]

def map_path(path):
    for (inp, out) in path_mapping:
        if out is None:
            if re.search(inp, path):
                return None
        else:
            path = re.sub(inp, out, path)
    return path

def map_diff(content, headerline):
    content = "#newline#".join(content)
    for (inp, out) in path_mapping:
        if out is not None:
            content = re.sub(inp, out, content)
    content = content.split("#newline#")
    diff = "#newline#".join(content[headerline:])

    # if "/examples/avr/can/mcp2515/main.cpp" in content:
    # print diff

    for (inp, out) in diff_mapping:
        diff = re.sub(inp, out, diff)
    return content[:headerline] + diff.split("#newline#")

with open(sys.argv[1], "r", errors="replace") as patchfile:
    content = patchfile.read()

files = [f.splitlines() for f in content.split("diff --git ")[1:]]
files = [(f[0].split(" b")[0][1:], f[0].split(" b")[1], f[1:]) for f in files]

for src, dst, diff in files:
    # if "/examples/avr/can/mcp2515/main.cpp" not in src:
    #     continue
    msrc = map_path(src)
    mdst = map_path(dst)
    new_file = ("new file" in diff[0])
    header_line = len(diff)
    for i,x in enumerate(diff):
        if x.startswith("@@"):
            header_line = i
            break
    mdiff = map_diff(diff, header_line)
    fsrc = mdiff[header_line-2][4 if new_file else 5:]
    fdst = mdiff[header_line-1][5:]

    # debug = "{}".format(src)
    # if src != dst:
    #     debug += " -> {}".format(dst)
    # if msrc is None:
    #     debug = "   !" + debug
    # elif src != msrc:
    #     debug = " " + debug + "\n {}".format(msrc)
    #     if msrc != mdst:
    #         debug += " -> {}".format(dst)
    # print(debug)

    mdiff = ["diff --git a{} b{}".format(msrc, mdst)] + mdiff

    if "-d" in sys.argv:
        if "/null" in fsrc and src != fdst:
            mvsrc, mvdst = src[1:], fdst[1:]
            print("mkdir -p {}; mv {} {}".format(os.path.dirname(mvdst), mvsrc, mvdst))
    else:
        if (msrc is None or mdst is None):
            continue
        print("\n".join(mdiff))



"""
0. work on the modm develop branch
1. git diff erstellen: git diff sha-since-last-merge xpcc/develop -- . ':!ext' > xpcc-develop.diff
2. git diff mit tools/xpcc_merge.py umschreiben: python tools/scripts/xpcc_merge.py xpcc-develop.diff > modm-develop.diff
3. work on the xpcc/develop branch: git checkout -b xpcc/develop
4. alle neuen files aus xpcc/develop verschieben: python tools/scripts/xpcc_merge.py xpcc-develop.diff -d | zsh
5. git commit
6. git checkout origin/develop
7. git merge --no-ff xpcc/develop
8. Alle lokalen changes _während_ des merge conflicts rauswerfen
9. umgeschriebenen git diff anwenden: git apply --reject -v --recount modm-develop.diff
10. alle diff rejects manuell auflösen (ist sehr viel Arbeit)
11. git merge fertig machen
12. copyright fixen: python3 tools/scripts/copyright.py -a -r sha-since-last-copyright-update > copyright_files.txt
"""