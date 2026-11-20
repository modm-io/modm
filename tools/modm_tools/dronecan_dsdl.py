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

r"""
### DroneCAN DSDL Code Generator

Generates header-only C serialization code for the DroneCAN (UAVCAN v0) data
types. It parses the DSDL with pydronecan and renders the code with jinja2:
`pip install dronecan jinja2`. The generated code packs the bits with
`canardEncodeScalar()`/`canardDecodeScalar()` of the DroneCAN fork of
libcanard, which the `modm:dronecan` module provides as `<dronecan/canard.h>`.

Every type gets a header named after its full name, e.g.
`uavcan.protocol.NodeStatus.h` with `struct uavcan_protocol_NodeStatus`,
`uavcan_protocol_NodeStatus_encode(msg, buffer)` returning the payload length,
and `uavcan_protocol_NodeStatus_decode(transfer, msg)` returning true if the
payload is invalid. Services get a `_req.h` and `_res.h` header with the
`...Request` and `...Response` structs, and a header with both. In C++, the
headers also specialize `modm::dronecan::Type<T>`.

```sh
python3 -m modm_tools.dronecan_dsdl -O generated/dronecan \
    --build uavcan.protocol.NodeStatus --build uavcan.protocol.GetNodeInfo \
    modm/ext/dronecan/dsdl/uavcan
```

Without `--build` all types are generated, otherwise only these types and their
dependencies. `--list` only prints the files that would be written, separated
by `;`, for build systems that need to know the outputs in advance.
Unchanged files are not written again.
"""

import os
import sys
from pathlib import Path


# -----------------------------------------------------------------------------
def c_name(full_name):
    return full_name.replace(".", "_")

def max_bytes(bitlen):
    return (bitlen + 7) // 8

def int_width(ptype):
    return next(w for w in (8, 16, 32, 64) if ptype.bitlen <= w)


class Part:
    """A message, or the request or response of a service."""
    def __init__(self, dtype, suffix=""):
        self.dtype = dtype
        self.suffix = suffix        # "", "Request", "Response"
        self.name = c_name(dtype.full_name) + suffix
        self.macro = c_name(dtype.full_name).upper() + ("_" + suffix.upper() if suffix else "")
        # the other half of a service
        self.other = {"Request": "Response", "Response": "Request"}.get(suffix)
        prefix = suffix.lower() + "_" if suffix else ""
        self.fields = getattr(dtype, prefix + "fields")
        self.constants = getattr(dtype, prefix + "constants")
        self.union = getattr(dtype, prefix + "union")
        self.max_size = max_bytes(getattr(dtype, "get_max_bitlen" + ("_" + suffix.lower() if suffix else ""))())
        self.header = dtype.full_name + {"Request": "_req", "Response": "_res"}.get(suffix, "") + ".h"
        self.dependencies = sorted({element(f.type).full_name + ".h" for f in self.fields
                                    if element(f.type).category == f.type.CATEGORY_COMPOUND})

def parts(dtype):
    if dtype.kind == dtype.KIND_SERVICE:
        return [Part(dtype, "Request"), Part(dtype, "Response")]
    return [Part(dtype)]

def headers(dtype):
    names = [p.header for p in parts(dtype)]
    if dtype.kind == dtype.KIND_SERVICE:
        names.append(dtype.full_name + ".h")
    return names

def element(t):
    """The type of the array elements or the type itself"""
    return t.value_type if t.category == t.CATEGORY_ARRAY else t


# -----------------------------------------------------------------------------
def c_type(t):
    if t.kind == t.KIND_BOOLEAN:
        return "bool"
    if t.kind == t.KIND_FLOAT:
        return "double" if t.bitlen == 64 else "float"
    return ("int" if t.kind == t.KIND_SIGNED_INT else "uint") + f"{int_width(t)}_t"

def field_type(t):
    """C type of a field and the array suffix of its name"""
    if t.category == t.CATEGORY_PRIMITIVE:
        return c_type(t), ""
    if t.category == t.CATEGORY_COMPOUND:
        return f"struct {c_name(t.full_name)}", ""
    vt = t.value_type
    elem = c_type(vt) if vt.category == vt.CATEGORY_PRIMITIVE else f"struct {c_name(vt.full_name)}"
    if t.mode == t.MODE_STATIC:
        return elem, f"[{t.max_size}]"
    return f"struct {{ {length_type(t)} len; {elem} data[{t.max_size}]; }}", ""

def length_type(t):
    return "uint8_t" if t.max_size < 256 else "uint16_t"

def saturate(t):
    """Integers narrower than their C type must be clamped when encoding"""
    return (t.kind in (t.KIND_UNSIGNED_INT, t.KIND_SIGNED_INT) and
            t.cast_mode == t.CAST_MODE_SATURATED and t.bitlen < int_width(t))

def constant_value(constant):
    value = constant.value
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, float):
        return repr(value)
    return str(value)

def body(text, depth):
    """Indents the rendered code of a macro by depth tabs"""
    return "\n".join("\t" * depth + line if line else line for line in str(text).rstrip("\n").split("\n"))


# -----------------------------------------------------------------------------
def environment():
    import jinja2
    env = jinja2.Environment(loader=jinja2.FileSystemLoader(Path(__file__).parent),
                             trim_blocks=True, lstrip_blocks=True, keep_trailing_newline=True,
                             undefined=jinja2.StrictUndefined)
    env.filters["body"] = body
    env.globals.update(c_name=c_name, c_type=c_type, field_type=field_type, length_type=length_type,
                       saturate=saturate, constant_value=constant_value)
    return env

def render(dtype, roots, env=None):
    """Returns the generated headers of a type as {file name: content}"""
    env = env or environment()
    # relative to the parent of its root namespace, e.g. uavcan/protocol/341.NodeStatus.uavcan
    root = next(r for r in roots if os.path.commonpath([r, dtype.source_file]) == r)
    source = os.path.relpath(dtype.source_file, os.path.dirname(root))
    template = env.get_template("dronecan_dsdl.h.in")
    files = {part.header: template.render(part=part, service=None, source=source)
             for part in parts(dtype)}
    if dtype.kind == dtype.KIND_SERVICE:
        files[dtype.full_name + ".h"] = template.render(service=dtype, source=source)
    return files

def select(types, names):
    """The full names of the types and all their dependencies"""
    selected = set(names) if names else set(types)
    unknown = selected - set(types)
    if unknown:
        raise ValueError("Unknown types: " + ", ".join(sorted(unknown)))
    todo = list(selected)
    while todo:
        for part in parts(types[todo.pop()]):
            for field in part.fields:
                t = element(field.type)
                if t.category == t.CATEGORY_COMPOUND and t.full_name not in selected:
                    selected.add(t.full_name)
                    todo.append(t.full_name)
    return sorted(selected)

def generate(namespaces, outdir, names=None):
    """Writes the headers of the types (all if None) and their dependencies.
    @return the paths of the headers"""
    import dronecan.dsdl
    roots = [os.path.abspath(ns) for ns in namespaces]
    types = {t.full_name: t for t in dronecan.dsdl.parse_namespaces(roots)}
    outdir = Path(outdir)
    outdir.mkdir(parents=True, exist_ok=True)
    env = environment()
    paths = []
    for name in select(types, names):
        for file, content in render(types[name], roots, env).items():
            path = outdir / file
            # only write changed files, so that builds do not recompile everything
            if not path.exists() or path.read_text() != content:
                path.write_text(content)
            paths.append(path)
    return paths

def outputs(namespaces, outdir, names=None):
    """The paths of the headers that generate() writes"""
    import dronecan.dsdl
    types = {t.full_name: t for t in dronecan.dsdl.parse_namespaces([os.path.abspath(ns) for ns in namespaces])}
    return [Path(outdir) / h for name in select(types, names) for h in headers(types[name])]


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Generate C code for DroneCAN DSDL types")
    parser.add_argument("-O", "--outdir", default=".", help="Output folder")
    parser.add_argument("--build", action="append", default=[], dest="types",
                        help="Only generate this type and its dependencies, may be repeated")
    parser.add_argument("--list", action="store_true", help="Only print the files that would be written")
    parser.add_argument("namespaces", nargs="+", help="Root namespace folders")
    args = parser.parse_args()

    try:
        if args.list:
            print(";".join(map(str, outputs(args.namespaces, args.outdir, args.types))))
        else:
            generate(args.namespaces, args.outdir, args.types)
    except ValueError as error:
        sys.exit(str(error))
