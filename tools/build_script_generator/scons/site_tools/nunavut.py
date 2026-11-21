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

# Cyphal DSDL to C headers with Nunavut (pip install nunavut).
# Generates header-only serialization code, so no sources are returned.
# The generated headers are declared as targets, so that SCons generates them
# before compiling the files that include them.

import os
import shutil
import subprocess
from pathlib import Path

import SCons.Errors


def _flags(env, namespaces, path):
    flags = ["--target-language", "c", "--language-standard", env["NUNAVUT_STD"],
             "--target-endianness", env["NUNAVUT_ENDIANNESS"], "--outdir", path]
    for ns in namespaces:
        flags += ["--lookup-dir", ns]
    return flags + env["NUNAVUTFLAGS"] + namespaces


def build_dsdl(env, namespaces, path):
    if not namespaces: return []
    nnvg = env.subst("$NNVG")
    if not shutil.which(nnvg):
        raise SCons.Errors.UserError("modm:nunavut needs the nnvg tool: pip install nunavut")
    namespaces = [os.path.abspath(ns) for ns in namespaces]
    flags = _flags(env, namespaces, path)
    # Ask nnvg which files it will write, so that SCons knows the targets
    outputs = subprocess.run([nnvg, "--list-outputs"] + flags, check=True,
                             capture_output=True, text=True).stdout
    targets = [o for o in outputs.strip().split(";") if o]
    sources = [str(p) for ns in namespaces for p in Path(ns).rglob("*.dsdl")]
    return env.Command(targets, sources,
                       env.Action([[nnvg] + flags], cmdstr="$NUNAVUT_COMSTR"))


def generate(env, **kw):
    env.SetDefault(NNVG=os.environ.get("NNVG", "nnvg"))
    env.SetDefault(NUNAVUT_STD="c11")
    env.SetDefault(NUNAVUT_ENDIANNESS="little")
    env.SetDefault(NUNAVUTFLAGS=[])
    env.SetDefault(NUNAVUT_COMSTR="Nunavut DSDL headers")
    env.AddMethod(build_dsdl, "NunavutDsdl")


def exists(env):
    return True
