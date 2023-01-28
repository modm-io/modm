#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016-2017, German Aerospace Center (DLR)
# Copyright (c) 2018, Niklas Hauser
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2022, Andrey Kunitsyn

import os

from SCons.Script import *
from modm_tools import elf2uf2

def elf2uf2_action(target, source, env):
    memranges = [{
        "start": m["start"],
        "end": m["start"] + m["size"],
        "type": "CONTENTS" if "flash" in m["name"] else "NO_CONTENTS"
    }
    for m in env["CONFIG_DEVICE_MEMORY"]]
    elf2uf2.convert(str(source[0]),
                    str(target[0]),
                    env["CONFIG_DEVICE_NAME"],
                    memranges)

def generate(env, **kw):
    builder_elf2uf2 = Builder(
        action=Action(elf2uf2_action, cmdstr="$UF2COMSTR"),
        suffix=".uf2",
        src_suffix=".elf")

    env.Append(BUILDERS={
        "UF2": builder_elf2uf2,
    })


def exists(env):
    return True

