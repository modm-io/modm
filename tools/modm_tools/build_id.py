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

"""
### GNU Build-ID

To extract the build ID from an ELF file:

```sh
python3 modm/modm_tools/build_id.py path/to/project.elf
fa8918e6971ed783b25bff0ad11a0541be47a933
```

To copy the ELF file to a artifact cache:

```sh
python3 modm/modm_tools/build_id.py path/to/project.elf --cache path/to/folder
# copies to `path/to/folder/fa8918e6971ed783b25bff0ad11a0541be47a933.elf`
```

(\* *only ARM Cortex-M targets*)
"""

from pathlib import Path
import shutil
from elftools.elf.elffile import ELFFile


def extract(source):
    with open(source, "rb") as src:
        build_id = ELFFile(src).get_section_by_name(".build_id")
        if build_id is not None:
            build_id = next(n['n_desc'] for n in build_id.iter_notes()
                            if n['n_type'] == "NT_GNU_BUILD_ID")
    return build_id


def cache_elf(source, path):
    build_id = extract(source)
    if build_id is not None:
        Path(path).mkdir(exist_ok=True, parents=True)
        destination = Path(path) / "{}.elf".format(build_id.lower())
        shutil.copy2(source, destination)
    else:
        raise ValueError("Unable to find Build ID for '{}'!".format(source))
    return build_id


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Extract GNU Build ID or cache ELF.')
    parser.add_argument(
            dest="source",
            metavar="ELF",
            help="The image to query.")
    parser.add_argument(
            "-c", "--cache",
            dest="cache_path",
            default=None,
            help="Cache folder for elf file.")

    args = parser.parse_args()
    if args.cache_path is None:
        build_id = extract(args.source)
    else:
        build_id = cache_elf(args.source, args.cache_path)
    print(build_id)
