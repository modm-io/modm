#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2023-2026, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

r"""
### JLink

Simply wraps JLinkGDBServer and issues the right command to program the target.

```sh
python3 -m modm_tools.jlink -device STM32F469NI path/to/project.elf
```

You can also reset the target:

```sh
python3 -m modm_tools.jlink -device STM32F469NI --reset
```

With several J-Links connected, select one by its serial number with
`-serial 001050033691`.

You can set the `MODM_JLINK_BINARY` environment variable to point this script to
a specific `JLinkGDBServer` binary:

```sh
export MODM_JLINK_BINARY=/path/to/other/JLinkGDBServer
```

(\* *only ARM Cortex-M targets*)
"""

import os
import time
import signal
import tempfile
import platform
import subprocess
from pathlib import Path

from . import gdb
from .backend import DebugBackend

# -----------------------------------------------------------------------------
class JLinkBackend(DebugBackend):
    def __init__(self, device, serial=None):
        super().__init__(":2331")
        self.device = device
        self.serial = serial
        self.process = None

    def start(self):
        self.process = call(self.device, blocking=False, silent=True, serial=self.serial)

    def stop(self):
        if self.process is not None:
            if "Windows" in platform.platform():
                os.kill(self.process.pid, signal.CTRL_BREAK_EVENT)
            else:
                os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
            os.waitpid(os.getpgid(self.process.pid), 0)
            self.process = None


def call(device, blocking=True, silent=False, verbose=False, serial=None):
    binary = "JLinkGDBServerCL"
    if "Windows" in platform.platform():
        binary += ".exe"
    binary = os.environ.get("MODM_JLINK_BINARY", binary)

    # JLinkGDBServer has no option for command strings, so set SetSkipDebugDeInit
    # (see _call_commander) from a J-Link script, which keeps the device scripts.
    script = Path(__file__).with_name("jlink.JLinkScript")
    command_jlink = f"{binary} -device {device} -if swd -noreset -nogui -nohalt -jlinkscriptfile \"{script}\""
    if serial:
        command_jlink += f" -select USB={serial}"
    if silent:
        command_jlink += " -silent"

    if verbose:
        print(command_jlink)

    kwargs = {"cwd": os.getcwd(), "shell": True}
    if blocking:
        return subprocess.call(command_jlink, **kwargs)

    # We have to start JLinkGDBServer in its own session ID, so that Ctrl-C in
    # GDB does not kill JLinkGDBServer.
    if "Windows" in platform.platform():
        kwargs["creationflags"] = subprocess.CREATE_NEW_PROCESS_GROUP
    else:
        kwargs["preexec_fn"] = os.setsid
    return subprocess.Popen(command_jlink, **kwargs)


def _call_commander(device, commands, verbose=True, serial=None):
    binary = "JLinkExe"
    if "Windows" in platform.platform():
        binary += ".exe"
    binary = os.environ.get("MODM_JLINK_COMMANDER_BINARY", binary)

    # By default J-Link clears DEMCR.TRCENA when closing the session, which stops
    # the DWT cycle counter that modm::delay_us() uses, so delays never finish.
    commands = ["exec SetSkipDebugDeInit = 1"] + commands
    with tempfile.NamedTemporaryFile(mode="w", suffix=".jlink") as script:
        script.write("\n".join(commands) + "\n")
        script.flush()
        command = [
            binary,
            "-device", device,
            "-if", "swd",
            "-speed", "4000",
            "-nogui", "1",
            "-ExitOnError", "1",
            "-CommandFile", script.name,
        ]
        if serial:
            command[1:1] = ["-USB", serial]
        if verbose: print(" ".join(command))
        return subprocess.call(command, cwd=os.getcwd())


# -----------------------------------------------------------------------------
def itm(device, baudrate=None):
    command_jlink = "JLinkSWOViewerCL -device {} -itmport 0".format(device)
    if baudrate: command_jlink += " -swofreq {}".format(baudrate)
    return subprocess.call(command_jlink, shell=True)


def rtt(backend, channel=0):
    # Start JLinkGDBServer in the background
    with backend.scope():
        time.sleep(0.5)
        subprocess.call(f"stty -icanon -echo; nc localhost {19021 + channel}; stty sane", shell=True)


# -----------------------------------------------------------------------------
def program(device, source, serial=None):
    return _call_commander(device, ["reset", f"loadfile {source}", "reset", "exit"], serial=serial)


def reset(device, serial=None):
    return _call_commander(device, ["reset", "exit"], serial=serial)


# -----------------------------------------------------------------------------
def add_subparser(subparser):
    parser = subparser.add_parser("jlink", help="Use JLink as Backend.")
    parser.add_argument(
            "-device",
            dest="device",
            required=True,
            help="Connect to this device.")
    parser.add_argument(
            "-serial",
            dest="serial",
            help="Use the J-Link with this serial number.")
    parser.set_defaults(backend=lambda args: JLinkBackend(args.device, args.serial))
    return parser


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(
        description="Program ELF file or reset device via JLink")
    parser.add_argument(
            dest="source",
            nargs="?",
            metavar="ELF")
    parser.add_argument(
            "-device",
            dest="device",
            required=True,
            help="Connect to this device.")
    parser.add_argument(
            "-r", "--reset",
            dest="reset",
            default=False,
            action="store_true",
            help="Reset device.")
    parser.add_argument(
            "-serial",
            dest="serial",
            help="Use the J-Link with this serial number.")

    args = parser.parse_args()
    if args.reset:
        exit(reset(args.device, args.serial))
    else:
        exit(program(args.device, os.path.abspath(args.source), args.serial))
