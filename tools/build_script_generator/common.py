#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import os
import platform
from collections import defaultdict

def common_source_files(env, buildlog):
    files_to_build = defaultdict(list)

    for operation in buildlog:
        repo = operation.module_name.split(":")[0]
        filename = operation.local_filename_out()
        _, extension = os.path.splitext(filename)

        if extension in [".c", ".cpp", ".cc", ".sx", ".S"]:
            files_to_build[repo].append(filename)

    for repo in files_to_build:
        files_to_build[repo].sort()
    return files_to_build

def common_target(target):
    core = target.get_driver("core")["type"]
    core = core.replace("fd", "").replace("f", "")
    mcu = target._properties.get("mcu", "")
    p = {
        "core": core,
        "mcu": mcu,
        "platform": target.identifier["platform"],
        "family": target.identifier["family"],
        "partname": target.partname,
    }
    return p

def common_memories(target):
    core_driver = target.get_driver("core")
    memories = []
    if "memory" in core_driver:
        memories.extend([
            {
                k:(int(v) if v.isdigit() else (int(v, 16) if v.startswith("0x") else v))
                for k, v in memory.items()
            }
            for memory in core_driver["memory"]
        ])
    return memories

def common_metadata_flags(buildlog, repo=None):
    flags = defaultdict(lambda: defaultdict(list))
    metadata = buildlog.metadata if repo is None else buildlog.repo_metadata
    for key, values in metadata.items():
        if key.startswith("flags."):
            key = key.split(".")[1:]
            if repo: values = values[repo];
            flags[key[0].upper()]["" if len(key) < 2 else key[1]] = list(values)
    return flags

def common_compiler_flags(compiler, target):
    flags = defaultdict(list)
    # flags for C **and** C++
    flags["flags.ccflags"] = [
        "-W",
        "-Wall",
        "-Wduplicated-cond",
        "-Werror=format",
        "-Werror=maybe-uninitialized",
        "-Werror=overflow",
        "-Werror=sign-compare",
        "-Wextra",
        "-Wlogical-op",
        "-Wpointer-arith",
        "-Wundef",
        # "-Wcast-align",
        # "-Wcast-qual",
        # "-Wmissing-declarations",
        # "-Wredundant-decls",
        # "-Wshadow",

        "-fdata-sections",
        "-ffunction-sections",
        "-fshort-wchar",
        "-funsigned-char",
        "-fwrapv",
        # "-fmerge-all-constants",

        "-g3",
        "-gdwarf",
    ]
    if compiler.startswith("gcc"):
        flags["flags.ccflags"] += [
            "-finline-limit=10000",
            "-funsigned-bitfields",
        ]
    flags["flags.ccflags.release"] = [
        "-Os",
    ]
    # not a valid profile
    # flags["flags.ccflags.fast"] = [
    #     "-O3",
    # ]
    flags["flags.ccflags.debug"] = [
        "-Og",
        "-fno-split-wide-types",
        "-fno-tree-loop-optimize",
        "-fno-move-loop-invariants",
    ]
    # flags only for C
    flags["flags.cflags"] = [
        "-Wimplicit",
        "-Wnested-externs",
        "-Wredundant-decls",
        "-Wstrict-prototypes",
        "-Wbad-function-cast",
        "-std=gnu11",
        # "-pedantic",
    ]
    # flags only for C++
    flags["flags.cxxflags"] = [
        "-Woverloaded-virtual",
        # "-Wctor-dtor-privacy",
        # "-Wnon-virtual-dtor",
        # "-Wold-style-cast",
        "-fstrict-enums",
        "-std=c++17",
        # "-pedantic",
    ]
    # flags only for Assembly
    flags["flags.asflags"] = [
        "-g3",
        "-gdwarf",
        # "-xassembler-with-cpp",
    ]
    # flags for the linker
    if target.identifier["family"] != "darwin":
        flags["flags.linkflags"] = [
            "-Wl,--fatal-warnings",
            "-Wl,--gc-sections",
            "-Wl,--relax",
            "-Wl,-Map,{target_base}.map,--cref",
        ]
    # C Preprocessor defines
    flags["flags.cppdefines"] = []
    flags["flags.cppdefines.debug"] = [
        "MODM_DEBUG_BUILD",
    ]
    # Architecture flags for C, C++, Assembly and **Linker**
    flags["flags.archflags"] = []

    # Target specific flags
    core = target.get_driver("core")["type"]
    if core.startswith("cortex-m"):
        cpu = core.replace("fd", "").replace("f", "")
        flags["flags.archflags"] += [
            "-mcpu={}".format(cpu),
            "-mthumb",
        ]
        single_precision = True
        fpu = core.replace("cortex-m", "").replace("+", "")
        if "f" in fpu:
            fpu_spec = {
                "4f": "-mfpu=fpv4-sp-d16",
                "7f": "-mfpu=fpv5-sp-d16",
                "7fd": "-mfpu=fpv5-d16",
            }[fpu]
            flags["flags.archflags"] += [
                "-mfloat-abi=hard",
                fpu_spec
            ]
            single_precision = ("-sp-" in fpu_spec)
        if single_precision:
            flags["flags.ccflags"] += [
                "-fsingle-precision-constant",
                "-Wdouble-promotion",
            ]
        flags["flags.cxxflags"] += [
            "-fno-exceptions",
            "-fno-unwind-tables",
            "-fno-rtti",
            "-fno-threadsafe-statics",
            "-fuse-cxa-atexit",
        ]
        flags["flags.linkflags"] += [
            "-Wl,--no-wchar-size-warning",
            "-Wl,-wrap,_calloc_r",
            "-Wl,-wrap,_free_r",
            "-Wl,-wrap,_malloc_r",
            "-Wl,-wrap,_realloc_r",
            "--specs=nano.specs",
            "--specs=nosys.specs",
            "-nostartfiles",
            "-L{linkdir}",
            "-Tlinkerscript.ld",
        ]

    elif core.startswith("avr"):
        flags["flags.archflags"] += [
            "-mmcu={}".format(target.partname),
        ]
        flags["flags.cxxflags"] += [
            "-fno-exceptions",
            "-fno-unwind-tables",
            "-fno-rtti",
            "-fno-threadsafe-statics",
        ]
        flags["flags.linkflags"] += [
            "-L{linkdir}",
            "-Tlinkerscript.ld",
        ]

    return flags

