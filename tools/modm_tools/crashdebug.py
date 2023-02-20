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
    def __init__(self, coredump, binary_path=None):
        self.coredump = coredump
        if binary_path is None:
            binary_path = os.environ.get("MODM_CRASHDEBUG_PATH")

        self.binary = "CrashDebug"
        if "Windows" in platform.platform():
            self.binary = "CrashDebug.exe"
        if binary_path is not None:
            self.binary = os.path.join(binary_path, self.binary)

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
            help="Folder of CrashDebug Binaries.")
    parser.add_argument(
            "--dump",
            dest="coredump",
            default="coredump.txt",
            help="Path to coredump file.")
    def build_backend(args):
        return CrashDebugBackend(args.coredump, args.binary_path)
    parser.set_defaults(backend=build_backend)
    return parser

