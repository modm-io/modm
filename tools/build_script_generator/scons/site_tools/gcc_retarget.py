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
from os.path import realpath, dirname
import shutil
import subprocess

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
    compiler = env.get('COMPILER', 'gcc')
    if compiler == 'gcc':
        env.Tool('gcc')
        env.Tool('g++')
    elif compiler == 'clang':
        env.Tool('clang')
        env.Tool('clang++')
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
    if compiler == 'gcc':
        env['CC'] = prefix + 'gcc' + suffix
        env['CXX'] =  prefix + 'g++' + suffix
    elif compiler == 'clang':
        env['CC'] = prefix + 'clang' + suffix
        env['CXX'] =  prefix + 'clang++' + suffix
    else:
        raise RuntimeError(f'Unsupported compiler: "{compiler}"')

    env['AR'] = prefix + 'ar'
    env['RANLIB'] = prefix + 'ranlib'
    env['AS'] = prefix + 'as' if suffix == '' else prefix + 'gcc' + suffix

    env['NM'] = prefix + 'nm'
    if compiler == 'gcc':
        for var, wrapper in [('AR', 'gcc-ar'), ('RANLIB', 'gcc-ranlib'), ('NM', 'gcc-nm')]:
            if shutil.which(prefix + wrapper + suffix) is not None:
                env[var] = prefix + wrapper + suffix
    elif compiler == 'clang':
        for var, wrapper in [('AR', 'llvm-ar'), ('RANLIB', 'llvm-ranlib'), ('NM', 'llvm-nm')]:
            tool = prefix + wrapper + suffix
            # Lookup matching llvm tools with clang command
            if shutil.which(tool) is None and shutil.which(env['CC']) is not None:
                tool = subprocess.run([env['CC'], '-print-prog-name=' + wrapper],
                                      capture_output=True, text=True).stdout.strip()
            if shutil.which(tool) is not None:
                env[var] = tool

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

    c_compiler_name = env["CC"]
    assert (c_compiler_path := shutil.which(c_compiler_name)), \
            f'Selected compiler "{c_compiler_name}" not found on PATH. ' \
            "Please add its installation directory to the PATH environment variable."
    env["GCC_PATH"] = dirname(dirname(realpath(c_compiler_path)))


def exists(env):
    return env.Detect(env["CC"])

