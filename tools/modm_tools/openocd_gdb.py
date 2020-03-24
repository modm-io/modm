#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import subprocess
import signal

def execute(env, mode):
    # Build GBD config and command
    gdb_configfiles = env.get("CONFIG_GDB", [])
    gdb_configfiles = ' '.join(map('-x "{}"'.format, gdb_configfiles))
    if mode == 'tui':
        gdb_command = 'arm-none-eabi-gdb ' \
                '-ex "layout split" ' \
                '-ex "focus cmd" ' \
                '{0} {1} ' \
                '-ex "refresh"'
    elif mode == 'gui':
        gdb_command = 'gdbgui ' \
                '-g arm-none-eabi-gdb ' \
                '--gdb-args="{0} {1}" ' \
                '{1}'
    else:
        print("No valid GDB mode selected: tui, gui")
        return 1

    # We have to start openocd in its own session ID, so that Ctrl-C in GDB
    # does not kill OpenOCD. See https://github.com/RIOT-OS/RIOT/pull/3619.
    openocd_configfiles = env.get("CONFIG_OPENOCD", [])
    openocd_configfiles = ' '.join(map('-f "{}"'.format, openocd_configfiles))
    openocd_command = 'openocd {} -c "log_output /dev/null"'.format(openocd_configfiles)
    openocd = subprocess.Popen(openocd_command, preexec_fn=os.setsid,
                               cwd=os.getcwd(), shell=True)

    # This call is now blocking
    source = env["CONFIG_SOURCES"][0]
    subprocess.call(gdb_command.format(gdb_configfiles, source),
                    cwd=os.getcwd(), shell=True)

    # Then kill just the background OpenOCD process
    os.killpg(os.getpgid(openocd.pid), signal.SIGTERM)

    return 0


if __name__ == "__main__":
    import argparse, sys

    parser = argparse.ArgumentParser(description='Run OpenOCD in background and GDB on top.')
    parser.add_argument(
            dest="elfs",
            metavar="ELF",
            type=str,
            nargs="+",
            help="The ELF files to use for debugging.")
    parser.add_argument(
            "--config-openocd",
            dest="config_openocd",
            type=str,
            action="append",
            default=[],
            help="Use these OpenOCD config files.")
    parser.add_argument(
            "--config-gdb",
            dest="config_gdb",
            type=str,
            action="append",
            default=[],
            help="Use these GDB init file.")
    parser.add_argument(
            "--mode",
            choices=['tui', 'gui'],
            dest="mode",
            help="Use GDB via TUI or GDBGUI.")
    args = parser.parse_args()

    env = {
        "CONFIG_OPENOCD": args.config_openocd,
        "CONFIG_GDB": args.config_gdb,
        "CONFIG_SOURCES": args.elfs,
    }
    exit(execute(env, args.mode))
