#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, 2023, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Black Magic Probe

This tool wraps GDB to program an ELF file onto a target connected to a BMP.
You can explictly pass the serial port, or let the tool guess it.

```sh
python3 -m modm_tools.bmp path/to/project.elf
# or choose the port explicitly
python3 -m modm_tools.bmp path/to/project.elf -p /dev/tty.usbserial-123
```

You can also reset the target:

```sh
python3 -m modm_tools.bmp --reset
```

(\* *only ARM Cortex-M targets*)
"""

from . import utils, gdb
from .backend import DebugBackend


# -----------------------------------------------------------------------------
class BlackMagicProbeBackend(DebugBackend):
    def __init__(self, port):
        if port == "auto":
            port = utils.guess_serial_port("bmp")
        if port is None:
            raise ValueError("Could not guess serial port!")
        super().__init__(port)

    def init(self, elf):
        return super().init(elf) + ["monitor swdp_scan", "attach 1"]


def program(port, source):
    backend = BlackMagicProbeBackend(port)
    commands = ["load", "compare-sections", "kill", "quit"]
    gdb.call(backend, source=source, commands=commands)


def reset(port):
    backend = BlackMagicProbeBackend(port)
    gdb.call(backend, commands=["kill", "quit"])


def add_subparser(subparser):
    parser = subparser.add_parser("bmp", help="Use Black Magic Probe as Backend.")
    parser.add_argument(
            "-p", "--port",
            dest="port",
            default="auto",
            help="Serial port of Black Magic Probe.")
    parser.set_defaults(backend=lambda args: BlackMagicProbeBackend(args.port))
    return parser


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse
    import os.path

    parser = argparse.ArgumentParser(
        description='Program ELF file or reset device via Black Magic Probe')
    parser.add_argument(
            dest="source",
            nargs="?",
            metavar="ELF")
    parser.add_argument(
            "-p", "--port",
            dest="port",
            default="auto",
            help="Serial port of Black Magic Probe.")
    parser.add_argument(
            "-r", "--reset",
            dest="reset",
            default=False,
            action="store_true",
            help="Reset device.")

    args = parser.parse_args()
    if args.reset:
        reset(args.port)
    else:
        program(args.port, os.path.abspath(args.source))


