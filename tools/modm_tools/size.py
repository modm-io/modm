#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, 2016-2017, German Aerospace Center (DLR)
# Copyright (c) 2017-2018, 2020, Niklas Hauser
# Copyright (c) 2018, Fabian Greif
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

"""
### Size Report

Inspects the ELF file and generates a size report of the static usage of the
device's memories. You must pass the available memory segments as a Python
dictionary:

```sh
python3 modm/modm_tools/size.py path/to/project.elf \\
    "[{'name': 'flash', 'access': 'rx', 'start': 134217728, 'size': 65536}, \\
    {'name': 'sram1', 'access': 'rwx', 'start': 536870912, 'size': 20480}]"

Program:   1.4 KiB (2.2% used)
(.build_id + .fastcode + .fastdata + .hardware_init + .rodata +
 .table.copy.intern + .table.heap + .table.zero.intern + .text + .vector_rom)

Data:      3.0 KiB (15.1% used) = 20 B static (0.1%) + 3072 B stack (15.0%)
(.bss + .fastdata + .stack)

Heap:     17.0 KiB (84.9% available)
(.heap1)
```

(\* *only ARM Cortex-M targets*)
"""

import textwrap
from collections import defaultdict
from pathlib import Path
from elftools.elf.elffile import ELFFile


TEMPLATE_SIZE = """\
Program: {rom_fmt:>9s} ({rom_p:2.1f}% used)
({rom_s})

Data:    {ram_fmt:>9s} ({ram_p:2.1f}% used) = {static} B static ({static_p:2.1f}%) + {stack} B stack ({stack_p:2.1f}%)
({ram_s})

Heap:  {heap_fmt:>11s} ({heap_p:2.1f}% available)
({heap_s})
"""


def human_readable_format(num, unit='B'):
    for prefix in ['', 'Ki', 'Mi', 'Gi', 'Ti']:
        if abs(num) < 1024.0:
            if prefix == '':
                # Align the output independent of whether a prefix is used
                return "%3.1f %s  " % (num, unit)
            else:
                return "%3.1f %s%s" % (num, prefix, unit)
        num /= 1024.0
    return "%.1f %s%s" % (num, 'Pi', unit)


def format(source, device_memories):
    memories = defaultdict(list)
    for memory in device_memories:
        if "w" in memory["access"]:
            memories["ram"].append(memory)
        else:
            memories["rom"].append(memory)

    memory_sections = []
    with open(source, "rb") as src:
        elffile = ELFFile(src)
        for section in elffile.iter_sections():
            s = {
                "name":  section.name,
                "vaddr": section["sh_addr"],
                "paddr": section["sh_addr"],
                "size": section["sh_size"],
            }
            if s["vaddr"] == 0 or s["size"] == 0: continue;
            for segment in elffile.iter_segments():
                if (segment["p_vaddr"] == s["vaddr"] and segment["p_filesz"] == s["size"]):
                    s["paddr"] = segment["p_paddr"]
                    break
            memory_sections.append(s)

    sections = defaultdict(list)
    totals = defaultdict(int)
    for s in memory_sections:
        if s["name"].startswith(".stack"):
            totals["stack"] += s["size"]
            sections["stack"].append(s["name"])
        elif s["name"].startswith(".heap"):
            totals["heap"] += s["size"]
            sections["heap"].append(s["name"])
        else:

            def is_in_memory(name):
                start = s[{"rom": "paddr", "ram": "vaddr"}[name]]
                return any(((m["start"] <= start) and
                            ((start + s["size"]) <= (m["start"] + m["size"])))
                            for m in memories[name])

            if is_in_memory("rom"):
                totals["rom"] += s["size"]
                sections["rom"].append(s["name"])
            if is_in_memory("ram"):
                totals["static"] += s["size"]
                sections["static"].append(s["name"])

    # create lists of the used sections for Flash and RAM
    sections["rom"] = sorted(sections["rom"])
    sections["ram"] = sorted(list(set(sections["static"] + sections["stack"])))
    sections["heap"] = sorted(sections["heap"])

    flash = sum(m["size"] for m in memories["rom"])
    ram = sum(m["size"] for m in memories["ram"])

    subs = {
        "ram": totals["static"] + totals["stack"],
        "rom_s": "\n ".join(textwrap.wrap(" + ".join(sections["rom"]), 80)),
        "ram_s": "\n ".join(textwrap.wrap(" + ".join(sections["ram"]), 80)),
        "heap_s": "\n ".join(textwrap.wrap(" + ".join(sections["heap"]), 80)),
        "rom_p": totals["rom"] / float(flash) * 100.0,
        "ram_p": (totals["static"] + totals["stack"]) / float(ram) * 100.0,
        "static_p": totals["static"] / float(ram) * 100.0,
        "stack_p": totals["stack"] / float(ram) * 100.0,
        "heap_p": totals["heap"] / float(ram) * 100.0
    }
    subs.update(totals)

    output = TEMPLATE_SIZE.format(ram_fmt=human_readable_format(subs["ram"]),
                                  rom_fmt=human_readable_format(subs["rom"]),
                                  heap_fmt=human_readable_format(subs["heap"]),
                                  **subs)
    return output


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Generate a size report.')
    parser.add_argument(
            dest="source",
            metavar="ELF")
    parser.add_argument(
            dest="memories")

    args = parser.parse_args()
    output = format(args.source, eval(args.memories))
    print(output)
