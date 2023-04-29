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

import os
import subprocess

from . import utils
from .backend import DebugBackend


# -----------------------------------------------------------------------------
def _gdb_command(backend, source=None, config=None, commands=None, ui=None):
    # Build GBD config and command
    args = ['-ex "set pagination off"']
    args += list(map('-ex "{}"'.format, utils.listify(backend.init(source))))
    args += list(map('-x "{}"'.format, utils.listify(config)))
    args += list(map('-ex "{}"'.format, utils.listify(commands)))
    args = " ".join(args)
    gdb = "arm-none-eabi-gdb"

    if ui is None:
        return f"{gdb} -nx -nh -batch {args} {source}"

    elif "cmd" in ui:
        return f"{gdb} {args} {source}"

    elif "tui" in ui:
        return f'{gdb} -tui -ex "layout split" -ex "focus cmd" {args} -ex "refresh" {source}'

    elif "gdbgui" in ui or "web" in ui:
        return f"gdbgui {source} --gdb-cmd='{gdb} {args} {source}'"

    raise ValueError("Unknown UI mode! '{}'".format(ui))


def call(backend, source=None, config=None, commands=None, ui=None):
    gdb_command = _gdb_command(backend, source, config, commands, ui)

    with backend.scope():
        try:
            return subprocess.call(gdb_command, cwd=os.getcwd(), shell=True)
        except KeyboardInterrupt:
            pass


# -----------------------------------------------------------------------------
def add_subparser(subparser):
    # Generic backend
    parser = subparser.add_parser("remote", help="Use a generic extended remote as Backend.")
    parser.add_argument(
            "--port",
            dest="port",
            default="localhost:3333",
            help="Connect to this host.")
    parser.set_defaults(backend=lambda args: DebugBackend(args.port))


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse, signal
    from . import openocd, crashdebug, bmp, jlink

    # Catch Ctrl-C before it terminates the GDB subprocess call
    def empty_signal_handler(sig, frame):
        pass
    signal.signal(signal.SIGINT, empty_signal_handler)

    parser = argparse.ArgumentParser(description='Run GDB in TUI or GUI mode')
    parser.add_argument(
            "--elf",
            dest="source",
            help="The ELF files to use for debugging.")
    parser.add_argument(
            "--ui",
            dest="ui",
            choices=["tui", "gdbgui", "web"],
            help="Use GDB via TUI or GDBGUI.")
    parser.add_argument(
            "-x",
            dest="config",
            action="append",
            help="Use these GDB init files.")
    parser.add_argument(
            "-ex",
            dest="commands",
            action="append",
            help="Extra GDB commands.")

    subparsers = parser.add_subparsers(title="Backend", dest="backend")

    # Add generic backends
    add_subparser(subparsers)
    # Add specific backends
    openocd.add_subparser(subparsers)
    crashdebug.add_subparser(subparsers)
    bmp.add_subparser(subparsers)
    jlink.add_subparser(subparsers)

    args = parser.parse_args()
    call(args.backend(args), args.source, args.config, args.commands, args.ui)
