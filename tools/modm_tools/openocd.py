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

You can also reset the target:

```sh
python3 modm/modm_tools/openocd.py -f modm/openocd.cfg --reset
```

(\* *only ARM Cortex-M targets*)
"""

import os
import time
import signal
import tempfile
import platform
import telnetlib
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
            if "Windows" in platform.platform():
                os.kill(self.process.pid, signal.CTRL_BREAK_EVENT)
            else:
                os.killpg(os.getpgid(self.process.pid), signal.SIGTERM)
            self.process = None


# -----------------------------------------------------------------------------
def call(commands=None, config=None, search=None, blocking=True, silent=False):
    commands = utils.listify(commands)
    config = utils.listify(config)
    search = utils.listify(search)
    if silent:
        null_file = "nul" if "Windows" in platform.platform() else "/dev/null"
        commands.append("log_output " + null_file)

    # Provide additional search paths via the OPENOCD_SCRIPTS environment variable
    # See http://openocd.org/doc/html/Running.html
    # os.environ.get("OPENOCD_SCRIPTS", "")

    command_openocd = "openocd {} {} {}".format(
        " ".join(map('-s "{}"'.format, search)),
        " ".join(map('-f "{}"'.format, config)),
        " ".join(map('-c "{}"'.format, commands))
    )
    # print(command_openocd)

    kwargs = {"cwd": os.getcwd(), "shell": True}
    if blocking:
        return subprocess.call(command_openocd, **kwargs)
    # We have to start openocd in its own session ID, so that Ctrl-C in GDB
    # does not kill OpenOCD. See https://github.com/RIOT-OS/RIOT/pull/3619.
    if "Windows" in platform.platform():
        kwargs["creationflags"] = subprocess.CREATE_NEW_PROCESS_GROUP
    else:
        kwargs["preexec_fn"] = os.setsid
    return subprocess.Popen(command_openocd, **kwargs)


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

def log_rtt(backend, channel=0):
    backend.commands.append("modm_rtt")
    # Start OpenOCD in the background
    with bem.Scope(backend) as b:
        time.sleep(0.5)
        with telnetlib.Telnet("localhost", 9090+channel) as tn:
            tn.interact()

# -----------------------------------------------------------------------------
def program(source, config=None, search=None):
    commands = ["modm_program {{{}}}".format(source)]
    call(commands=commands, config=config, search=search)

def reset(config=None, search=None):
    commands = ["reset", "shutdown"]
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

    parser = argparse.ArgumentParser(
        description='Program ELF file or reset device via OpenOCD')
    parser.add_argument(
            dest="source",
            nargs="?",
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
    parser.add_argument(
            "-r", "--reset",
            dest="reset",
            default=False,
            action="store_true",
            help="Reset device.")

    args = parser.parse_args()
    if args.reset:
        reset(config=args.config, search=args.searchdirs)
    else:
        program(source=os.path.abspath(args.source),
                config=args.config, search=args.searchdirs)
