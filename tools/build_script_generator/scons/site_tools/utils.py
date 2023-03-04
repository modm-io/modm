#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014, 2016-2017, German Aerospace Center (DLR)
# Copyright (c) 2018, 2023, Niklas Hauser
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Authors:
# - 2013-2014, 2016-2017, Fabian Greif (DLR RY-AVS)
# - 2018, Niklas Hauser

import os
import sys
import os.path

from modm_tools import utils
from SCons.Script import *

def _listify(obj):
    if obj is None:
        return list()
    if isinstance(obj, SCons.Node.NodeList):
        return obj
    if isinstance(obj, (list, tuple, set)):
        return list(obj)
    if hasattr(obj, "__iter__") and not hasattr(obj, "__getitem__"):
        return list(obj)
    return [obj, ]


def listify(env, *objs):
    return [entry for obj in objs for entry in _listify(obj)]

def subst_list(env, *keys):
    values = []
    for key in keys:
        values += env.Listify(env.get(key, []))
    return [env.subst(v) for v in values]

def remove_from_list(env, identifier, to_remove):
    """
    Remove strings from a list.

    E.g.
    env.RemoveFromList('CXXFLAGS_warning', ['-Wold-style-cast'])
    """
    if identifier.startswith('$'):
        raise Exception("Identifier '%s' must not start with '$'!" % identifier)

    l = env.subst('$' + identifier)
    if isinstance(l, str):
        l = l.split(' ')
    for r in _listify(to_remove):
        if r in l:
            l.remove(r)
    env[identifier] = l


def filtered_glob(env, pattern, omit=None, ondisk=True, source=False, strings=False):
    if omit is None:
        omit = []

    results = []
    for p in _listify(pattern):
        results.extend(filter(lambda f: os.path.basename(f.path) not in omit,
                              env.Glob(p)))
    return results


def run_program(env, program):
    return env.Command('phony_target',
                       program,
                       '@"%s"' % program[0].abspath)


def phony_target(env, **kw):
    for target, action in kw.items():
        env.AlwaysBuild(env.Alias(target, [], action))


def compiler_version(env):
    return utils.compiler_version(env.subst(env["CC"]))


def artifact_firmware(env, source):
    firmware = ARGUMENTS.get("firmware", None)
    if firmware:
        try:
            int(firmware, 16) # Validate hexadecimal string
            source = os.path.join(env["CONFIG_ARTIFACT_PATH"], "{}.elf".format(firmware.lower()))
        except:
            source = firmware
    return source

def always_build_action(env, action, cmdstr, source=None):
    action = Action(action, cmdstr)
    caller_name = sys._getframe(2).f_code.co_name
    return env.AlwaysBuild(env.Alias(caller_name, source, action))

# -----------------------------------------------------------------------------
def generate(env, **kw):
    env.Append(ENV={'PATH': os.environ['PATH']})

    env.AddMethod(remove_from_list, 'RemoveFromList')
    env.AddMethod(filtered_glob, 'FilteredGlob')

    env.AddMethod(run_program, 'Run')
    env.AddMethod(phony_target, 'Phony')

    env.AddMethod(listify, 'Listify')
    env.AddMethod(subst_list, 'SubstList')

    env.AddMethod(always_build_action, 'AlwaysBuildAction')

    env.AddMethod(compiler_version, 'CompilerVersion')

    env.AddMethod(artifact_firmware, 'ChooseFirmware')


def exists(env):
    return True

