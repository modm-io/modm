#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### AvrDude

This tool simply wraps the `avrdude` command to provide two features:

- guessing the serial port when a baudrate was set.
- inspecting the ELF file to decide whether EEPROM needs to be programmed.

```sh
python3 modm/modm_tools/avrdude.py -p m328p -c arduino -P auto -b 57600 \\
                                   path/to/project.elf
```

[Fuses stored in the ELF file](http://avr-libc.nongnu.org/user-manual/group__avr__fuse.html)
can be programmed by passing `--fuse` arguments:

```sh
python3 modm/modm_tools/avrdude.py -p m328p -c stk500v2 path/to/project.elf \\
                                   --fuse hfuse --fuse lfuse --fuse efuse
```

(\* *only AVR targets*)
"""

import os
import subprocess
if __name__ == "__main__":
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from modm_tools import utils
from elftools.elf.elffile import ELFFile, NoteSection

# -----------------------------------------------------------------------------
def program(source, device, programmer, port=None, baudrate=None, fuses=None, options=None):
    command = ["avrdude", "-V", "-p {}".format(device), "-c {}".format(programmer)]

    # Attempt to find a serial port automatically
    if port == "auto" and baudrate:
        port = utils.guess_serial_port(programmer)

    if port is not None:
        command.append("-P {}".format(port))
    if baudrate is not None:
        command.append("-b {}".format(baudrate))

    with open(str(source), "rb") as src:
        elffile = ELFFile(src)
        sections = [section.name for section in elffile.iter_sections()]

    command.append('-U flash:w:"{}":e'.format(source))
    if ".eeprom" in sections:
        command.append('-U eeprom:w:"{}":e'.format(source))
    for fuse in utils.listify(fuses):
        command.append('-U {}:w:"{}":e'.format(fuse, source))
    command += utils.listify(options)

    command = " ".join(command)
    # print(command)
    subprocess.call(command, cwd=os.getcwd(), shell=True)


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Program ELF file via AvrDude')
    parser.add_argument(
            dest="source",
            metavar="ELF")
    parser.add_argument(
            "-p",
            dest="partno",
            required=True)
    parser.add_argument(
            "-c",
            dest="programmer",
            required=True)
    parser.add_argument(
            "-b",
            dest="baudrate")
    parser.add_argument(
            "-P",
            dest="port",
            default="auto")
    parser.add_argument(
            "--fuse",
            dest="fuses",
            choices=["lfuse", "hfuse", "efuse"],
            action="append",
            help="The fuses to write to the target.")

    args = parser.parse_args()
    program(args.source, args.partno, args.programmer,
            fuses=args.fuses, baudrate=args.baudrate, port=args.port)
