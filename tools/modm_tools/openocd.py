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
### OpenOCD

Simply wraps OpenOCD and issues the right command to program the target.

```sh
python3 modm/modm_tools/openocd.py -f modm/openocd.cfg path/to/project.elf
```

(\* *only ARM Cortex-M targets*)
"""

import os
import signal
import tempfile
import subprocess
if __name__ == "__main__":
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from modm_tools import utils, backend as bem

# -----------------------------------------------------------------------------
class OpenOcdBackend:
    def __init__(self, commands=None, config=None, search=None):
        self.commands = utils.listify(commands)
        self.config = utils.listify(config)
        self.search = utils.listify(search)
        self.process = None

    def init(self, elf):
        return ["target extended-remote :3333"]

    def start(self):
        self.process = call(self.commands, self.config, self.search,
                            blocking=False, silent=True)

    def stop(self):
        if self.process is not None:
            os.killpg(os.getpgid(self.process.pid), signal.SIGTERM)
            self.process = None


# -----------------------------------------------------------------------------
def call(commands=None, config=None, search=None, blocking=True, silent=False):
    commands = utils.listify(commands)
    config = utils.listify(config)
    search = utils.listify(search)
    if silent:
        commands.append("log_output /dev/null")

    # Provide additional search paths via the OPENOCD_SCRIPTS environment variable
    # See http://openocd.org/doc/html/Running.html
    # os.environ.get("OPENOCD_SCRIPTS", "")

    command_openocd = "openocd {} {} {}".format(
        " ".join(map('-s "{}"'.format, search)),
        " ".join(map('-f "{}"'.format, config)),
        " ".join(map('-c "{}"'.format, commands))
    )
    # print(command_openocd)

    if blocking:
        return subprocess.call(command_openocd, cwd=os.getcwd(), shell=True)
    else:
        # We have to start openocd in its own session ID, so that Ctrl-C in GDB
        # does not kill OpenOCD. See https://github.com/RIOT-OS/RIOT/pull/3619.
        return subprocess.Popen(command_openocd, cwd=os.getcwd(), shell=True,
                                preexec_fn=os.setsid)


# -----------------------------------------------------------------------------
def log_itm(backend, fcpu, baudrate=None):
    baudrate = "" if baudrate is None else baudrate
    if not fcpu:
        raise ValueError("fcpu must be the CPU/HCLK frequency!")

    with tempfile.NamedTemporaryFile() as tmpfile:
        command = "modm_itm_log {} {} {}".format(tmpfile.name, fcpu, baudrate)
        backend.commands.append(command)
        # Start OpenOCD in the background
        with bem.Scope(backend) as b:
            # Start a blocking call to monitor the log file
            # TODO: yield out new log lines in the future
            try:
                subprocess.call("tail -f {}".format(tmpfile.name),
                                cwd=os.getcwd(), shell=True)
            except KeyboardInterrupt:
                pass


# -----------------------------------------------------------------------------
def program(source, config=None, search=None):
    commands = ["modm_program {{{}}}".format(source)]
    call(commands=commands, config=config, search=search)


# -----------------------------------------------------------------------------
def add_subparser(subparser):
    parser = subparser.add_parser("openocd", help="Use OpenOCD as Backend.")
    parser.add_argument(
            "-f",
            dest="oconfig",
            action="append",
            help="Use these OpenOCD config files.")
    parser.add_argument(
            "-s",
            dest="seachdirs",
            action="append",
            help="Search in these paths for config files.")
    parser.add_argument(
            "-c",
            dest="ocommands",
            action="append",
            help="Extra OpenOCD commands.")
    def build_backend(args):
        return OpenOcdBackend(args.ocommands, args.oconfig, args.seachdirs)
    parser.set_defaults(backend=build_backend)
    return parser


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Program ELF file via OpenOCD')
    parser.add_argument(
            dest="source",
            metavar="ELF")
    parser.add_argument(
            "-f",
            dest="config",
            action="append",
            help="Use these OpenOCD config files.")
    parser.add_argument(
            "-s",
            dest="searchdirs",
            action="append",
            help="Search in these paths for config files.")

    args = parser.parse_args()
    program(source=os.path.abspath(args.source),
            config=args.config, search=args.searchdirs)
