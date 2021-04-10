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

import os
import subprocess
if __name__ == "__main__":
    import sys
    sys.path.append(os.path.dirname(os.path.dirname(__file__)))

from modm_tools import utils, backend as bem
from modm_tools import openocd, crashdebug, bmp
# -----------------------------------------------------------------------------
def call(source=None, config=None, commands=None, backend=None, ui=None):
    if backend is None:
        backend = bem.Empty()
    source = utils.listify(source)
    if len(source): source = source[0]
    else: source = None

    # Build GBD config and command
    args  = list(map('-ex "{}"'.format, utils.listify(backend.init(source))))
    args += list(map('-x "{}"'.format, utils.listify(config)))
    args += list(map('-ex "{}"'.format, utils.listify(commands)))

    # Build complete command string
    if ui is None:
        gdb_command = 'arm-none-eabi-gdb -nx --batch {args} {source}'
    elif 'tui' in ui:
        gdb_command = 'arm-none-eabi-gdb -tui ' \
                '-ex "layout split" ' \
                '-ex "focus cmd" ' \
                '{args} ' \
                '-ex "refresh" ' \
                '{source}'
    elif 'web' in ui:
        gdb_command = 'gdbgui {source} ' \
                "--gdb-cmd='arm-none-eabi-gdb {args} {source}'"
    else:
        raise ValueError("Unknown UI mode! '{}'".format(ui))

    gdb_command = gdb_command.format(source="" if source is None else source,
                                     args=" ".join(args))

    # Start Backend in the background
    with bem.Scope(backend) as b:
        try:
            # This call is now blocking
            subprocess.call(gdb_command, cwd=os.getcwd(), shell=True)
        except KeyboardInterrupt:
            pass

    return True


def add_subparser(subparser):
    # Generic backend
    parser = subparser.add_parser("remote", help="Use a generic extended remote as Backend.")
    parser.add_argument(
            "--host",
            dest="host",
            default="localhost",
            help="Connect to this host.")
    def build_backend(args):
        return bem.ExtendedRemote(args.host)
    parser.set_defaults(backend=build_backend)

# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Run GDB in TUI or GUI mode')
    parser.add_argument(
            dest="source",
            metavar="ELF",
            help="The ELF files to use for debugging.")
    parser.add_argument(
            "--ui",
            dest="ui",
            choices=["tui", "web"],
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

    args = parser.parse_args()
    call(source=args.source, backend=args.backend(args), ui=args.ui,
         config=args.config, commands=args.commands)
