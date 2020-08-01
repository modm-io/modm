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

import os, platform

class CrashDebugBackend:
    def __init__(self, binary_path, coredump):
        self.coredump = coredump

        crashdebug = "lin64/CrashDebug"
        if "Windows" in platform.platform():
            crashdebug = "win32/CrashDebug.exe"
        elif "Darwin" in platform.system():
            crashdebug = "osx64/CrashDebug"
        self.binary = os.path.join(binary_path, crashdebug)

    def init(self, elf):
        init = ["set target-charset ASCII",
                "target remote | {} --elf {} --dump {}"
                .format(self.binary, elf, self.coredump)]
        return init

    def start(self):
        pass

    def stop(self):
        pass


def add_subparser(subparser):
    parser = subparser.add_parser("crashdebug", help="Use CrashDebug as Backend.")
    parser.add_argument(
            "--binary-path",
            dest="binary_path",
            required=True,
            help="Folder of CrashDebug Binaries.")
    parser.add_argument(
            "--dump",
            dest="coredump",
            default="coredump.txt",
            help="Path to coredump file.")
    def build_backend(args):
        return CrashDebugBackend(args.binary_path, args.coredump)
    parser.set_defaults(backend=build_backend)
    return parser

