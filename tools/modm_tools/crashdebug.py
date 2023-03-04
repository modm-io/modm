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

import os, platform
from .backend import DebugBackend


class CrashDebugBackend(DebugBackend):
    def __init__(self, coredump):
        super().__init__()
        self.coredump = coredump
        self.binary = "CrashDebug"
        if "Windows" in platform.platform():
            self.binary = "CrashDebug.exe"
        self.binary = os.environ.get("MODM_CRASHDEBUG_BINARY", self.binary)

    def init(self, elf):
        return ["set target-charset ASCII",
                "target remote | {} --elf {} --dump {}"
                .format(self.binary, elf, self.coredump)]


def add_subparser(subparser):
    parser = subparser.add_parser("crashdebug", help="Use CrashDebug as Backend.")
    parser.add_argument(
            "--dump",
            dest="coredump",
            default="coredump.txt",
            help="Path to coredump file.")
    parser.set_defaults(backend=lambda args: CrashDebugBackend(args.coredump))
    return parser

