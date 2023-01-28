#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2022, Lucas Moesch
# Copyright (c) 2022, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import SCons.Action
import SCons.Builder
import SCons.Util
from SCons.Script import Dir, File

import os.path
import platform
import sys

import nanopb_builder

# -----------------------------------------------------------------------------
# Support for nanopb protobuf compilation.
# This suits as a wrapper for nanopb's own SCons build integration, which is
# integrated via the nanopb_builder module. The SCons integration can be found
# in modm/ext/nanopb/nanopb/tests/site_scons/site_tools/nanopb.py.
# -----------------------------------------------------------------------------

def relpath(path, start):
    os_relpath = os.path.relpath(path, start)
    if os_relpath[0] == ".":
        return os_relpath
    else:
        return os.path.join(".", os_relpath)


def _nanopb_proto_actions(source, target, env, for_signature):
    esc = env["ESCAPE"]
    outpath = env.get("path", ".")
    actionlist = []
    for protofile in source:
        # Make protoc build inside the SConscript directory
        srcfile = esc(os.path.basename(str(protofile)))
        targetfile = os.path.relpath(os.path.join(outpath, protofile.name.replace(".proto", ".pb.cpp")), ".")
        srcdir = esc(relpath(os.path.dirname(str(protofile)), "."))
        include_dirs = [srcdir]

        for d in env["PROTOCPATH"]:
            d = env.GetBuildPath(d)
            if not os.path.isabs(d): d = os.path.relpath(d, ".")
            include_dirs.append(esc(d))

        # when generating .pb.cpp sources, instead of pb.h generate .pb.hpp headers
        nanopb_flags = ["--source-extension=.cpp", "--header-extension=.hpp"]
        if env["NANOPBFLAGS"]: nanopb_flags.append(env["NANOPBFLAGS"])
        nanopb_flags = ",".join(nanopb_flags)

        actionlist.append(SCons.Action.CommandAction(
            "$PROTOC $PROTOCFLAGS -I{} --nanopb_out={} --nanopb_opt={} {}" \
            .format(" -I".join(include_dirs), outpath, nanopb_flags, srcfile),
            cmdstr=env.subst("$NANOPB_MESSAGES_COMSTR", target=targetfile, source=protofile)))
    return SCons.Action.ListAction(actionlist)


def _nanopb_proto_emitter(target, source, env):
    outpath = env.get("path", ".")
    targets = []
    for protofile in source:
        targets.append(os.path.join(outpath, protofile.name.replace(".proto", ".pb.cpp")))
        targets.append(os.path.join(outpath, protofile.name.replace(".proto", ".pb.hpp")))
        options = os.path.splitext(str(protofile))[0] + ".options"
        if os.path.exists(options):
            env.Depends(target=protofile, dependency=options)

    return targets, source


def build_proto(env, sources, path):
    if not sources: return []
    files = env.NanopbProtoCpp(sources, path=path)
    sources = [file for file in files if file.name.endswith(".cpp")]
    return sources


def generate(env, **kw):
    env["NANOPB"] = os.path.abspath("./scons/site_tools/nanopb_builder")
    env["PROTOC"] = nanopb_builder._detect_protoc(env)
    env["PROTOCFLAGS"] = nanopb_builder._detect_protocflags(env)

    env.SetDefault(NANOPBFLAGS = "")
    env.SetDefault(PROTOCPATH = [".", os.path.join(env["NANOPB"], "generator", "proto")])

    env["BUILDERS"]["NanopbProtoCpp"] = \
        SCons.Builder.Builder(
            generator = _nanopb_proto_actions,
            suffix = ".pb.cpp",
            src_suffix = ".proto",
            emitter = _nanopb_proto_emitter)

    env.AddMethod(build_proto, "NanopbProtofile")

def exists(env):
    return True
