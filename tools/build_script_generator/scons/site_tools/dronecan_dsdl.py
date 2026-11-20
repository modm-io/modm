#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2026, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

# DroneCAN DSDL to header-only C code with modm_tools.dronecan_dsdl, which
# parses the DSDL with pydronecan and renders the code with jinja2.
# The generated headers are declared as targets, so that SCons generates them
# before compiling the files that include them.
# Not named dronecan.py, since SCons puts this folder on sys.path, where it
# would shadow pydronecan.

import os
import sys
from pathlib import Path

import SCons.Errors
from modm_tools import dronecan_dsdl


def build_dsdl(env, namespaces, path, types=None):
    if not namespaces: return []
    namespaces = [os.path.abspath(ns) for ns in namespaces]
    try:
        import jinja2
        targets = [str(t) for t in dronecan_dsdl.outputs(namespaces, path, types)]
    except ImportError as error:
        # SCons may run with another Python than lbuild, so name the right one
        raise SCons.Errors.UserError(f"modm:ext:dronecan needs pydronecan and jinja2 in the Python of SCons: "
                                     f"{sys.executable} -m pip install dronecan jinja2\n{error}")
    except ValueError as error:
        raise SCons.Errors.UserError(f"modm:ext:dronecan: {error}")
    tool = Path(dronecan_dsdl.__file__)
    sources = [str(p) for ns in namespaces for p in Path(ns).rglob("*.uavcan")] + \
              [str(tool), str(tool.with_suffix(".h.in"))]
    def action(target, source, env):
        dronecan_dsdl.generate(namespaces, path, types)
    return env.Command(targets, sources, env.Action(action, cmdstr="$DRONECAN_COMSTR"))


def generate(env, **kw):
    env.SetDefault(DRONECAN_COMSTR="DroneCAN DSDL headers")
    env.AddMethod(build_dsdl, "DronecanDsdl")


def exists(env):
    return True
