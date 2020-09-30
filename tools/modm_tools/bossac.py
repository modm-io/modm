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
### BOSSA

This tool simply wraps the `bossac` command line tool to guess the serial port.

```sh
python3 modm/modm_tools/bossac.py -p auto --offset=0x2000 -e \\
                                   path/to/project.bin
```

(\* *only SAM targets*)
"""

import os
import subprocess
if __name__ == "__main__":
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from modm_tools import utils
from elftools.elf.elffile import ELFFile, NoteSection

# -----------------------------------------------------------------------------
def program(source, offset=None, port=None, erase=False, options=None):
    command = ["bossac", "-b", "-R", "-a"]

    # Attempt to find a serial port automatically
    if port == "auto":
        port = utils.guess_serial_port("bossac")

    if port is not None:
        command.append("--port={}".format(port))

    if offset is not None:
        command.append("--offset={}".format(offset))

    if erase:
        command.append("-e")

    command.extend(utils.listify(options))
    command.extend(["-v", "-w", str(source)])

    command = " ".join(command)
    # print(command)
    subprocess.call(command, cwd=os.getcwd(), shell=True)


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Program ELF file via BOSSA')
    parser.add_argument(
            dest="source",
            metavar="BIN")
    parser.add_argument(
            "-p",
            dest="port",
            default="auto")
    parser.add_argument(
            "--offset",
            dest="offset")
    parser.add_argument(
            "-e",
            dest="erase",
            default=False)

    args = parser.parse_args()
    program(args.source, offset=args.offset, port=args.port, erase=args.erase)
