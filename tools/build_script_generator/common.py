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

import os, re
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

common_source_flag_map = {
    "c": (r"\.[ci]$", ["ccflags", "archflags", "cppdefines", "cflags"]),
    "cpp": (r"\.[cC][pxci+]+$", ["ccflags", "archflags", "cppdefines", "cxxflags"]),
    "asm": (r"(\.[sS]x?|\.asm)$", ["archflags", "cppdefines", "asflags"]),
}

def common_source_files(env):
    """
    Builds a list of files that need to be compiled per repository
    (\\* *post-build only*).

    :returns: a dictionary of sorted lists of filenames, keyed by repository.
    """
    files_to_build = defaultdict(list)

    for operation in env.buildlog:
        suffix = os.path.splitext(operation.filename)[-1]
        if any(re.search(pattern[0], suffix) for pattern in common_source_flag_map.values()):
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

def common_avrdude_options(env):
    """
    Merges the default AvrDude options with the user options
    (\\* *post-build only*):

    - `avrdude_programmer`
    - `avrdude_port`
    - `avrdude_baudrate`
    - `avrdude_options`

    :returns: options dictionary
    """
    option_programmer = env.get(":build:avrdude.programmer")
    option_port = env.get(":build:avrdude.port")
    option_baudrate = env.get(":build:avrdude.baudrate")
    option_options = env.get(":build:avrdude.options")

    if not len(option_programmer):
        option_programmer = env.collector_values(":build:default.avrdude.programmer", option_programmer)[0]
    if not len(option_port):
        option_port = env.collector_values(":build:default.avrdude.port", option_port)[0]
    if not option_baudrate:
        option_baudrate = env.collector_values(":build:default.avrdude.baudrate", option_baudrate)[0]
    if not option_options:
        option_options = env.collector_values(":build:default.avrdude.options", option_options)[0]
    options = {
        "avrdude_programmer": option_programmer,
        "avrdude_port": option_port,
        "avrdude_baudrate": option_baudrate,
        "avrdude_options": option_options,
    }
    return options

def common_collect_flags_for_scope(env, scope_filter=None):
    """
    Scans the collections for module compile flags.
    Converts them into SCons-compatible names and places them into a dictionary
    of the form: `flags[filename][name][profile] = list(values)` (\\* *post-build only*).

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
    """
    Returns the common compile flags for a given compiler and target
    in the form: `flags[name(.profile)] = list(compiler flags)`.

    :param compiler: Currently supports only "GCC"
    :param target: the target identifier
    :returns: compiler flags dictionary
    """
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
        "-Wno-redundant-decls",
        # "-Wcast-align",
        # "-Wcast-qual",
        # "-Wmissing-declarations",
        # "-Wredundant-decls",
        # "-Wshadow",

        "-fdata-sections",
        "-ffunction-sections",
        "-funsigned-char",
        "-fwrapv",
        # "-fmerge-all-constants",
        "-ffile-prefix-map={project_source_dir}=.",
        "-ffile-prefix-map={gccpath}=.",

        "-g3",
        "-gdwarf-3",
    ]

    if target.identifier["platform"] not in ["hosted"]:
        flags["ccflags"].append("-fshort-wchar")
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
        "-std=c++20",
        "-Wno-volatile",  # volatile is deprecated in C++20 but lots of our external code uses it...
        # "-pedantic",
    ]
    # flags only for Assembly
    flags["asflags"] = [
        "-g3",
        "-gdwarf-3",
        # "-xassembler-with-cpp",
    ]
    # flags for the linker
    if target.identifier["family"] != "darwin":
        flags["linkflags"] = [
            "-Wl,--fatal-warnings",
            "-Wl,--gc-sections",
            "-Wl,--relax",
        ]
    if target.identifier["family"] not in ["darwin", "windows"]:
        flags["linkflags"] += [
            "-Wl,-Map,{target_base}.map,--cref"
        ]
    # C Preprocessor defines
    flags["cppdefines"] = []
    if target.identifier["family"] == "windows":
        # Required for extended <inttypes.h> types
        flags["cppdefines"] += ["__STDC_FORMAT_MACROS"]
    flags["cppdefines.debug"] = [
        "MODM_DEBUG_BUILD",
    ]

    return flags

