#!/usr/bin/env python3
# 
# Copyright (c) 2009, Roboterclub Aachen e.V.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Roboterclub Aachen e.V. nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
        output = " ".join(args).replace("\\", "\\\\") + "\n"
        os.write(fd, bytearray(output, "utf-8"))
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
