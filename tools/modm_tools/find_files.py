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

from pathlib import Path
from os.path import join, relpath, isabs, dirname
if __name__ == "__main__":
    import sys
    sys.path.append(dirname(dirname(__file__)))

from modm_tools import utils

# -----------------------------------------------------------------------------
HEADER = [".h", ".hh", ".hpp"]
SOURCE = [".cpp", ".cxx", ".cc", ".c", ".sx", ".S", ".s"]

def scan(basepath, extensions, ignorePaths=None, ignoreFiles=None):
	ignoreFiles = utils.listify(ignoreFiles);
	if ignorePaths is not None and len(ignorePaths):
		ignorePaths = [relpath(p, basepath) if isabs(p) else p for p in utils.listify(ignorePaths)]
		ignoreFiles += [join(f, "**/*") for f in ignorePaths];
	files = set(str(p) for ext in utils.listify(extensions) for p in Path(basepath).glob("**/*{}".format(ext)))
	files -= set(str(p) for ign in utils.listify(ignoreFiles) for p in Path(basepath).glob(ign))
	return sorted(list(files))


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Find header/source files recursively.')
    parser.add_argument(
            dest="path",
            help="The path to search in.")
    parser.add_argument(
            "-ext", "--extensions",
            dest="extensions",
            choices=["header", "source", "all"],
            default="all",
            help="What extensions to look for.")
    parser.add_argument(
            "-ip", "--ignore-path",
            dest="ignore_paths",
            action="append",
            help="Ignore these paths.")
    parser.add_argument(
            "-if", "--ignore-file",
            dest="ignore_files",
            action="append",
            help="Ignore these files.")

    args = parser.parse_args()

    exts = {"source": SOURCE, "header": HEADER, "both": HEADER+SOURCE}
    files = scan(args.path, exts[args.extensions],
                 args.ignore_paths, args.ignore_files)
    print("\n".join(files))
