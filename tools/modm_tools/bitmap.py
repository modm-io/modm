#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2009-2012, Fabian Greif
# Copyright (c) 2012, Sascha Schade
# Copyright (c) 2012, 2014, 2017, 2020, Niklas Hauser
# Copyright (c) 2013, Kevin Läufer
# Copyright (c) 2016, Daniel Krebs
# Copyright (c) 2017, Michael Thies
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Bitmap

This tool can convert P1 `.pbm` files into C++ source code.

```sh
python3 modm/modm_tools/bitmap.py image.pbm --outpath .
# creates `image.hpp` and `image.cpp` in the CWD
```
"""

import string
import os
import re
import math
from pathlib import Path

# -----------------------------------------------------------------------------
TEMPLATE_HEADER = """
#pragma once
#include <modm/architecture/interface/accessor.hpp>

namespace bitmap
{{

/**
 * Generated bitmap
 *
 * - Width  : {width}
 * - Height : {height}
 */
EXTERN_FLASH_STORAGE(uint8_t {name}[]);

}}
"""

TEMPLATE_SOURCE = """
#include <modm/architecture/interface/accessor.hpp>

namespace bitmap
{{

FLASH_STORAGE(uint8_t {name}[]) =
{{
    {width}, {height},
    {array}
}};

}}
"""

# -----------------------------------------------------------------------------
def generate(image_data, name):
    if image_data[0:3] != "P1\n":
        raise ValueError("Input needs to be a portable bitmap in ASCII format (file descriptor 'P1')!")

    image_data = image_data[3:]

    while image_data[0] == "#" or image_data[0] in string.whitespace:
        # switch to the next line
        image_data = image_data[image_data.find("\n") + 1:]

    result = re.match(r"^(\d+) (\d+)\n", image_data)
    if not result:
        raise ValueError("bad format!")

    width = int(result.group(1))
    height = int(result.group(2))

    rows = int(math.ceil(height / 8.0))

    # now we finally have the raw data
    image_data = image_data[result.end():]
    image_data = image_data.replace("\n", "")

    data = []
    for y in range(rows):
        data.append([0 for row in range(width)])

    for y in range(height):
        for x in range(width):
            index = x + y * width
            if image_data[index] == "1":
                data[y // 8][x] |= 1 << (y % 8)

    array = []
    for y in range(rows):
        line = []
        for x in range(width):
            line.append("0x%02x," % data[y][x])
        array.append(" ".join(line))

    substitutions = {
        "name": name,
        "width": width,
        "height": height,
        "array": "\n\t".join(array),
    }

    header = TEMPLATE_HEADER.format(**substitutions)
    source = TEMPLATE_SOURCE.format(**substitutions)

    return (header, source)


def convert(image, outpath):
    name = Path(image).stem
    header, source = generate(Path(image).read_text(), name)
    outpath = Path(outpath) / name
    (outpath.with_suffix(".hpp")).write_text(header)
    (outpath.with_suffix(".cpp")).write_text(source)


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Convert PBM image to source code')
    parser.add_argument(
            dest="image",
            metavar="PBM",
            help="The image to convert to C++ code.")
    parser.add_argument(
            "-o", "--outpath",
            dest="outpath",
            default=".",
            help="Output folder for header and source file.")

    args = parser.parse_args()
    convert(args.image, args.outpath)
