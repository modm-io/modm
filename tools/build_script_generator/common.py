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
import itertools
from collections import defaultdict

common_build_flags = {
    "ccflags": ("Compiler flags for both C and C++ sources",
                ["See [Options that Control Optimization](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)",
                 "See [Options to Request or Suppress Warnings](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)",
                 "See [Options for Debugging Your Program](https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html)"]),
    "cflags": ("Compiler flags only for C sources",
               ["See [Options Controlling C Dialect](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)"]),
    "cxxflags": ("Compiler flags only for C++ sources",
                 ["See [Options Controlling C++ Dialect](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Dialect-Options.html)"]),
    "asflags": ("Assembler flags",
                ["See [Assembler Options](https://gcc.gnu.org/onlinedocs/gcc/Assembler-Options.html)"]),
    "archflags": ("Compiler flags related to the target architecture",
                  ["See [Machine-Dependent Options](https://gcc.gnu.org/onlinedocs/gcc/Submodel-Options.html)"]),
    "linkflags": ("Linker flags",
                  ["See [Options for Linking](https://gcc.gnu.org/onlinedocs/gcc/Link-Options.html)"]),
    "cppdefines": ("Preprocessor definitions",
                   ["Accepted values are `NAME` or `NAME=DEFINITION`.",
                    "See `-D name=definition` in [Preprocessor Options](https://gcc.gnu.org/onlinedocs/gcc/Preprocessor-Options.html)"]),
}

common_build_profiles = [
    "release",
    "debug",
]

def _fmt_flag_descr(flag, profile):
    flags = common_build_flags[flag]
    descr = [flags[0], ""]
    if len(profile):
        descr[0] += " ({} profile)".format(profile[1:])
    if "flags" in flag:
        descr.append("Flags must start with '-'!")
    descr += flags[1]
    return "\n".join(descr)

common_build_flag_names = {
    flag+profile: _fmt_flag_descr(flag, profile)
        for flag in common_build_flags
            for profile in ([""] + ["."+p for p in common_build_profiles])
}

def common_source_files(env):
    """
    Builds a list of files that need to be compiled per repository.

    :param buildlog: the buildlog object available in the post_build step
    :returns: a dictionary of sorted lists of filenames, keyed by repository.
    """
    files_to_build = defaultdict(list)

    for operation in env.buildlog:
        if os.path.splitext(operation.filename)[-1] in [".c", ".cpp", ".cc", ".sx", ".S"]:
            files_to_build[operation.repository].append(operation.filename)

    for repo in files_to_build:
        files_to_build[repo].sort()
    return files_to_build

def common_target(env):
    """
    Extracts common properties from a modm:target device:
      - platform
      - family
      - partname
      - core
      - mcu (AVR only)

    :returns: a dictionary of common properties.
    """
    device = env["modm:target"]
    core = device.get_driver("core")["type"]
    core = core.replace("fd", "").replace("f", "")
    mcu = device._properties.get("mcu", "")
    p = {
        "core": core,
        "mcu": mcu,
        "platform": device.identifier["platform"],
        "family": device.identifier["family"],
        "partname": device.partname,
    }
    return p

def common_memories(env):
    """
    Extracts the memory map of the device.
    A memory region is a dictionary containing:
      - `name` of region
      - `start` address of region
      - `size` of region
      - `access` of region

    :returns: a list of memory regions.
    """
    device = env["modm:target"]
    core_driver = device.get_driver("core")
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


def common_collect_flags_for_scope(env, scope_filter=None):
    """
    Scans the collections for module compile flags.
    Converts them into SCons-compatible names and places them into a dictionary
    of the form: flags[filename][name][profile] = list(values).

    :param env: the post_build step env
    :param scope_filter: the collection scope filter
    :returns: compile flags dictionary
    """
    flags = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))
    for flag in common_build_flags:
        for profile in common_build_profiles + [""]:
            name = "modm:build:{}".format(flag)
            if len(profile): name = "{}.{}".format(name, profile);
            for scope, values in env.collector(name).items():
                if (scope_filter is None or scope_filter(scope)) and len(values):
                    flags[scope.filename][flag][profile].extend(values)
    return flags


def common_compiler_flags(compiler, target):
    flags = defaultdict(list)
    # flags for C **and** C++
    flags["ccflags"] = [
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
        flags["ccflags"] += [
            "-finline-limit=10000",
            "-funsigned-bitfields",
        ]
    flags["ccflags.release"] = [
        "-Os",
    ]
    # not a valid profile
    # flags["ccflags.fast"] = [
    #     "-O3",
    # ]
    flags["ccflags.debug"] = [
        "-Og",
        "-fno-split-wide-types",
        "-fno-tree-loop-optimize",
        "-fno-move-loop-invariants",
    ]
    # flags only for C
    flags["cflags"] = [
        "-Wimplicit",
        # "-Wnested-externs",
        "-Wredundant-decls",
        "-Wstrict-prototypes",
        "-Wbad-function-cast",
        "-std=gnu11",
        # "-pedantic",
    ]
    # flags only for C++
    flags["cxxflags"] = [
        "-Woverloaded-virtual",
        # "-Wctor-dtor-privacy",
        # "-Wnon-virtual-dtor",
        # "-Wold-style-cast",
        "-fstrict-enums",
        "-std=c++17",
        # "-pedantic",
    ]
    # flags only for Assembly
    flags["asflags"] = [
        "-g3",
        "-gdwarf",
        # "-xassembler-with-cpp",
    ]
    # flags for the linker
    if target.identifier["family"] != "darwin":
        flags["linkflags"] = [
            "-Wl,--fatal-warnings",
            "-Wl,--gc-sections",
            "-Wl,--relax",
            "-Wl,--build-id=sha1",
            # "-Wl,-Map,{target_base}.map,--cref",
        ]
    # C Preprocessor defines
    flags["cppdefines"] = []
    flags["cppdefines.debug"] = [
        "MODM_DEBUG_BUILD",
    ]
    # Architecture flags for C, C++, Assembly and **Linker**
    flags["archflags"] = []

    # Target specific flags
    core = target.get_driver("core")["type"]
    if core.startswith("cortex-m"):
        cpu = core.replace("fd", "").replace("f", "").replace("+", "plus")
        flags["archflags"] += [
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
            flags["archflags"] += [
                "-mfloat-abi=hard",
                fpu_spec
            ]
            single_precision = ("-sp-" in fpu_spec)
        if single_precision:
            flags["ccflags"] += [
                "-fsingle-precision-constant",
                "-Wdouble-promotion",
            ]
        flags["cxxflags"] += [
            "-fno-exceptions",
            "-fno-unwind-tables",
            "-fno-rtti",
            "-fno-threadsafe-statics",
            "-fuse-cxa-atexit",
        ]
        flags["linkflags"] += [
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
        flags["archflags"] += [
            "-mmcu={}".format(target.partname),
        ]
        flags["cxxflags"] += [
            "-fno-exceptions",
            "-fno-unwind-tables",
            "-fno-rtti",
            "-fno-threadsafe-statics",
        ]
        flags["linkflags"] += [
            "-L{linkdir}",
            "-Tlinkerscript.ld",
        ]

    return flags

