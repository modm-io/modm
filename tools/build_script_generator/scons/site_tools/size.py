#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013, 2016-2017, German Aerospace Center (DLR)
# Copyright (c) 2017-2018, Niklas Hauser
# Copyright (c) 2018, Fabian Greif
# Copyright (c) 2020, Raphael Lehmann
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2013, 2016-2017, Fabian Greif (DLR RY-AVS)
# - 2017-2018, Niklas Hauser
# - 2018, Fabian Greif

# USAGE:
#
# Reads and matches ELF sections and segments to the physical memories
# passed in CONFIG_DEVICE_MEMORY which needs to be a list of dictionaries:
#
#     env["CONFIG_DEVICE_MEMORY"] = [
#         {"name": "flash", "start": 0x08000000, "size": 2097152, "access": "rx"},
#         {"name": "ccm", "start": 0x10000000, "size": 65536, "access": "rw"},
#         {"name": "sram1", "start": 0x20000000, "size": 163840, "access": "rwx"}
#     ]

from SCons.Script import *

def show_size(env, source, alias='__size'):
    if env.has_key('CONFIG_DEVICE_MEMORY'):
        def size_action(target, source, env):
            from modm_tools import size
            print(size.format(source[0].abspath, env['CONFIG_DEVICE_MEMORY']))
            return 0
        action = Action(size_action, cmdstr="$SIZECOMSTR")
    elif env.has_key('CONFIG_DEVICE_NAME') and ("avr" in env["ARCHITECTURE"]):
        # the mcu parameter is an AVR specific feature
        action = Action("$SIZE -C --mcu=$CONFIG_DEVICE_NAME %s" % source[0].path,
                        cmdstr="$SIZECOMSTR")
    else:
        # use the raw output of the size tool
        action = Action("$SIZE %s" % source[0].path, cmdstr="$SIZECOMSTR")

    return env.AlwaysBuild(env.Alias(alias, source, action))

def generate(env, **kw):
    env.AddMethod(show_size, 'Size')

def exists(env):
    return True
