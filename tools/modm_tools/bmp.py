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
### Black Magic Probe

This tool wraps GDB to program an ELF file onto a target connected to a BMP.
You can explictly pass the serial port, or let the tool guess it.

```sh
python3 modm/modm_tools/bmp.py path/to/project.elf
# or choose the port explicitly
python3 modm/modm_tools/bmp.py path/to/project.elf -p /dev/tty.usbserial-123
```

You can also reset the target:

```sh
python3 modm/modm_tools/bmp.py --reset
```

(\* *only ARM Cortex-M targets*)
"""

if __name__ == "__main__":
    import os, sys
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from modm_tools import utils

class BlackMagicProbeBackend:
    def __init__(self, port):
        if port == "auto":
            port = utils.guess_serial_port("bmp")
        if port is None:
            raise ValueError("Could not guess serial port!")
        self.port = port

    def init(self, elf):
        return ["target extended-remote {}".format(self.port),
                "monitor swdp_scan", "attach 1"]

    def start(self):
        pass

    def stop(self):
        pass

# -----------------------------------------------------------------------------
def program(source, port=None):
    from modm_tools import gdb
    backend = BlackMagicProbeBackend(port=port)
    commands = ["load", "compare-sections", "kill", "quit"]
    gdb.call(source=source, backend=backend, commands=commands)

def reset(port=None):
    from modm_tools import gdb
    backend = BlackMagicProbeBackend(port=port)
    commands = ["kill", "quit"]
    gdb.call(backend=backend, commands=commands)

# -----------------------------------------------------------------------------
def add_subparser(subparser):
    parser = subparser.add_parser("bmp", help="Use Black Magic Probe as Backend.")
    parser.add_argument(
            "-p", "--port",
            dest="port",
            default="auto",
            help="Serial port of Black Magic Probe.")
    def build_backend(args):
        return BlackMagicProbeBackend(args.port)
    parser.set_defaults(backend=build_backend)
    return parser

# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

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
        reset(port=args.port)
    else:
        program(source=os.path.abspath(args.source), port=args.port)


