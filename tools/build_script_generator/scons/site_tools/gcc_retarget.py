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
# - 2016-2017, Fabian Greif (DLR RY-AVS)
# - 2018, Niklas Hauser

import os

from SCons.Script import *

def strip_binary(env, target, source, options="--strip-unneeded"):
    return env.Command(target,
                       source,
                       Action("$STRIP {} -o $TARGET $SOURCE".format(options),
                              cmdstr="$STRIPCOMSTR"))

def list_symbols(env, source):
    action = Action("$NM $SOURCE -S -C --size-sort -td",
                    cmdstr="$SYMBOLSCOMSTR")
    return env.AlwaysBuild(env.Alias("__symbols", source, action))


def generate(env, **kw):
    env.Tool('gcc')
    env.Tool('g++')
    env.Tool('gnulink')
    env.Tool('ar')
    env.Tool('as')
    env.Tool('utils')

    # Define executable name of the compiler
    path = env.get('COMPILERPATH', '')
    prefix = env.get('COMPILERPREFIX', '')
    suffix = env.get('COMPILERSUFFIX', '')
    if suffix != '' and not suffix.startswith('-'):
        suffix = '-' + suffix

    prefix = path + prefix
    env['CC'] = prefix + 'gcc' + suffix
    env['CXX'] = prefix + 'g++' + suffix
    env['AR'] = prefix + 'ar'
    env['RANLIB'] = prefix + 'ranlib'
    if suffix == '':
        env['AS'] = prefix + 'as'
        env['NM'] = prefix + 'nm'
    else:
        env['AS'] = prefix + 'gcc' + suffix
        env['NM'] = prefix + 'gcc-nm' + suffix
        if sys.platform != "darwin":
            env['AR'] = prefix + 'gcc-ar' + suffix
            env['RANLIB'] = prefix + 'gcc-ranlib' + suffix

    env['OBJCOPY'] = prefix + 'objcopy'
    env['OBJDUMP'] = prefix + 'objdump'
    env['SIZE'] = prefix + 'size'
    env['STRIP'] = prefix + 'strip'

    env['LINK'] = env['CXX']

    builder_hex = Builder(
        action=Action("$OBJCOPY -O ihex $SOURCE $TARGET",
        cmdstr="$HEXCOMSTR"),
        suffix=".hex",
        src_suffix="")

    builder_bin = Builder(
        action=Action("$OBJCOPY -O binary $SOURCE $TARGET",
        cmdstr="$BINCOMSTR"),
        suffix=".bin",
        src_suffix="")

    builder_listing = Builder(
        action=Action("$OBJDUMP -x -S -l -w $SOURCE > $TARGET",
        cmdstr="$LSSCOMSTR"),
        suffix=".lss",
        src_suffix="")

    env.Append(BUILDERS={
        'Hex': builder_hex,
        'Bin': builder_bin,
        'Listing': builder_listing
    })

    env.AddMethod(strip_binary, 'Strip')
    env.AddMethod(list_symbols, 'Symbols')


def exists(env):
    return True

