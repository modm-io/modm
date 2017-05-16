#!/usr/bin/env python2
# -*- coding: utf-8 -*-
#
# Copyright (c) 2011-2012, Georgi Grinshpun
# Copyright (c) 2012, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2015, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------
# 
# DESCRIPTION
# 
# This is a tool compile programs for the AVR microcontrollers from Atmel.
# The 'avr-gcc' is used as compiler.
# 
# EXAMPLE
# 
# A simple SConstruct file to compile a file named 'main.c' for an ATmega644
# can look as follows:
# 
#    env = Environment(tools = ['avr'],
#             AVR_DEVICE = 'atmega644',
#             AVR_CLOCK = 16000000)
#    
#    program = env.Program('main.c')
#    
#    env.Hex(program)
#    env.Listing(program)
#
# BUILDERS
# 
# The following builders are available:
# 
# Object()      .c   -> .o
# Object()      .cpp -> .o
# Object()      .S   -> .o
# Library()     .o   -> .a
# Program()     .o   -> .elf
# Hex()         .elf -> .hex
#               .elf -> .eep
# Listing()     .elf -> .lss
#
# The 'Program' builder will automatically call the 'Object' builder so
# it's no need to use it explicitly. Just call 'Program' with all your
# source files.
#
# TODO
# 
# - Make the optimization level adjustable

import os
from SCons.Script import *
import SCons.Subst
import tempfile
import os

# -----------------------------------------------------------------------------
# We use an adapted Version of this class from 'SCons/Platform/__init__.py' for
# Windows because GCC requires all backslashes inside a paramter file to be escaped.
class TempFileMungeWindows(object):
    def __init__(self, cmd):
        self.cmd = cmd
    
    def __call__(self, target, source, env, for_signature):
        if for_signature:
            return self.cmd
        
        # do the expansion.
        cmd = env.subst_list(self.cmd, SCons.Subst.SUBST_CMD, target, source)[0]
        
        # create a file for the arguments
        (fd, tmp) = tempfile.mkstemp('.lnk', text=True)
        native_tmp = SCons.Util.get_native_path(os.path.normpath(tmp))
        
        args = list(map(SCons.Subst.quote_spaces, cmd[1:]))
        output = " ".join(args).replace("\\", "\\\\")
        os.write(fd, output + "\n")
        os.close(fd)
        if SCons.Action.print_actions:
            print("TempFileMungeWindows: Using tempfile "+native_tmp+" for command line:\n"+
                  str(cmd[0]) + " " + " ".join(args))
        return [cmd[0], '@"' + native_tmp + '"\ndel', '"' + native_tmp + '"']

# -----------------------------------------------------------------------------
def generate(env, **kw):
    if str(Platform()) == "win32":
        # use a tempfile for the arguments, otherwise the command line string might be to long
        # for windows to handle (maximum length is 2048 characters)
        env['TEMPFILE'] = TempFileMungeWindows
        env['LINKCOM'] = "${TEMPFILE('%s')}" % env['LINKCOM']
        env['ARCOM'] = "${TEMPFILE('%s')}" % env['ARCOM']

# -----------------------------------------------------------------------------
def exists(env):
    return True
