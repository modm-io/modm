#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020-2021, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

"""
### Information Tool

This tool generates a set of source files containing information about the
repository state.

You can use the `--check-rebuild` flag to only write the output file when the
information changed. This prevents unnecessary rebuilding and relinking.

!!! warning "Respect developers privacy"
    This information is placed into the firmware in **cleartext**, so it will
    be trivial to extract from a memory dump. Consider this information public
    as soon as it is uploaded to your target. Make sure you only use the
    information you absolutely need!


#### Git Information

```sh
python3 -m modm_tools.info -o git_info.c -t git        [--check-rebuild]
python3 -m modm_tools.info -o git_info.c -t git_status [--check-rebuild]
```

The `git_info(directory)` function returns a dictionary with these values:

- `MODM_GIT_SHA`: commit hash: `%H`.
- `MODM_GIT_SHA_ABBR`: short commit hash: `%h`.
- `MODM_GIT_SUBJECT`: commit subject as text: `%s`.
- `MODM_GIT_AUTHOR`: author name: `%an`.
- `MODM_GIT_AUTHOR_EMAIL`: author email: `%ae`.
- `MODM_GIT_AUTHOR_DATE`: authoring date: `%ad`.
- `MODM_GIT_AUTHOR_DATE_TIMESTAMP`: authoring date as Unix timestamp: `%at`.
- `MODM_GIT_COMMITTER`: committer name: `%cn`.
- `MODM_GIT_COMMITTER_EMAIL`: committer email: `%ce`.
- `MODM_GIT_COMMITTER_DATE`: committer date: `%cd`.
- `MODM_GIT_COMMITTER_DATE_TIMESTAMP`: committer das as Unix timestamp: `%ct`.
- `MODM_GIT_CONFIG_USER_NAME`: local user name: `user.name`.
- `MODM_GIT_CONFIG_USER_EMAIL`: local user email: `user.email`.

The `git_info(directory, with_status=True)` function returns these additional values:

- `MODM_GIT_MODIFIED`: number of modified files: `M`.
- `MODM_GIT_ADDED`: number of added files: `A`.
- `MODM_GIT_DELETED`: number of deleted files: `D`.
- `MODM_GIT_RENAMED`: number of renamed files: `R`.
- `MODM_GIT_COPIED`: number of copied files: `C`.
- `MODM_GIT_UNTRACKED`: number of untracked files: `?`.

This example project is showing an unclean repository state with uncommitted
changes. This can give you a few hints as to where a firmware came from and help
you pinpoint the source of a bug or feature.

```
Local Git User:
Name:  Name Surname
Email: name.surname@example.com
Last Commit:
SHA:             1b5a9a642857182161a615039c92907e59881614
Abbreviated SHA: 1b5a9a642
Subject:         wip

Author:
Name:      Name Surname
Email:     name.surname@example.com
Date:      Tue Jul 17 22:23:20 2018 +0200
Timestamp: 1531859000

Committer:
Name:      Name Surname
Email:     name.surname@example.com
Date:      Tue Jul 17 22:23:20 2018 +0200
Timestamp: 1531859000

File Status:
Modified:  10
Added:     0
Deleted:   0
Renamed:   0
Copied:    0
Untracked: 6
```


#### Build Information

```sh
python3 -m modm_tools.info -o build_info.c -t build --compiler=gcc [--check-rebuild]
```

Generates a files with these values defined as `const char *` strings:

- `MODM_BUILD_PROJECT_NAME`: as defined in the `modm:build:project.name` option.
- `MODM_BUILD_MACHINE`: machine information.
- `MODM_BUILD_USER`: user information.
- `MODM_BUILD_OS`: OS version string (best effort, may not be useful!).
- `MODM_BUILD_COMPILER`: compiler information.

Example output on macOS:

```
Project:  build_info
Machine:  name.local
User:     user
OS:       macOS 10.14.6 (x86_64)
Compiler: g++-10 10.2.0
```
"""

import re
import jinja2
import locale
import getpass
import platform
import subprocess

from pathlib import Path
from git import Repo

TEMPLATE_SOURCE = Path(__file__).parent / "info.c.in"


def git_info(directory, with_status=False):
    info = {
        "MODM_GIT_SHA": "Unknown",
        "MODM_GIT_SHA_ABBR": "Unknown",
        "MODM_GIT_AUTHOR": "Unknown",
        "MODM_GIT_AUTHOR_EMAIL": "Unknown",
        "MODM_GIT_AUTHOR_DATE": "Unknown",
        "MODM_GIT_AUTHOR_DATE_TIMESTAMP": "Unknown",
        "MODM_GIT_COMMITTER": "Unknown",
        "MODM_GIT_COMMITTER_EMAIL": "Unknown",
        "MODM_GIT_COMMITTER_DATE": "Unknown",
        "MODM_GIT_COMMITTER_DATE_TIMESTAMP": "Unknown",
        "MODM_GIT_SUBJECT": "Unknown",
        "MODM_GIT_CONFIG_USER_NAME": "Unknown",
        "MODM_GIT_CONFIG_USER_EMAIL": "Unknown",
    }
    if with_status:
        info.update({
            "MODM_GIT_MODIFIED": -1,
            "MODM_GIT_ADDED": -1,
            "MODM_GIT_DELETED": -1,
            "MODM_GIT_RENAMED": -1,
            "MODM_GIT_COPIED": -1,
            "MODM_GIT_UNTRACKED": -1,
        })
    try:
        repo = Repo(directory, search_parent_directories=True)
        # Last Commit Values
        commit = repo.commit()
        info["MODM_GIT_SHA"]                      = str(commit.hexsha)
        info["MODM_GIT_SHA_ABBR"]                 = str(commit.hexsha[:10])
        info["MODM_GIT_AUTHOR"]                   = str(commit.author.name)
        info["MODM_GIT_AUTHOR_EMAIL"]             = str(commit.author.email)
        info["MODM_GIT_AUTHOR_DATE"]              = str(commit.authored_datetime)
        info["MODM_GIT_AUTHOR_DATE_TIMESTAMP"]    = commit.authored_date
        info["MODM_GIT_COMMITTER"]                = str(commit.committer.name)
        info["MODM_GIT_COMMITTER_EMAIL"]          = str(commit.committer.email)
        info["MODM_GIT_COMMITTER_DATE"]           = str(commit.committed_datetime)
        info["MODM_GIT_COMMITTER_DATE_TIMESTAMP"] = commit.committed_date
        info["MODM_GIT_SUBJECT"]                  = str(commit.summary)
        # Git Config
        config = repo.config_reader()
        info["MODM_GIT_CONFIG_USER_NAME"]  = str(config.get_value("user", "name", "Unknown"))
        info["MODM_GIT_CONFIG_USER_EMAIL"] = str(config.get_value("user", "email", "Unknown"))
        if with_status:
            # File status
            diff = repo.index.diff(None)
            def default(change_type):
                try: return len(list(diff.iter_change_type(change_type)));
                except: return -1;
            info["MODM_GIT_MODIFIED"]  = len(diff)
            info["MODM_GIT_ADDED"]     = default("A")
            info["MODM_GIT_DELETED"]   = default("D")
            info["MODM_GIT_RENAMED"]   = default("R")
            info["MODM_GIT_COPIED"]    = default("C")
            info["MODM_GIT_UNTRACKED"] = len(repo.untracked_files)
    except:
        pass

    info = {k:v if isinstance(v, int) else v.encode("ascii", errors="replace").decode("ascii")
            for k,v in info.items()}

    return info


def build_info(directory=None, cxx_compiler=None):
    info = {
        "MODM_BUILD_MACHINE": "Unknown",
        "MODM_BUILD_USER": "Unknown",
        "MODM_BUILD_COMPILER": "Unknown",
        "MODM_BUILD_OS": "Unknown",
    }
    try:
        info["MODM_BUILD_MACHINE"] = platform.node()
        info["MODM_BUILD_USER"] = getpass.getuser()
        # Generate OS String
        if platform.system() == "Linux" and "linux_distribution" in dir(platform):
            os = " ".join(platform.linux_distribution())
        elif platform.system() == "Darwin":
            os = "macOS {0} ({2})".format(*platform.mac_ver())
        else:
            os = platform.platform(terse=True)
        info["MODM_BUILD_OS"] = os
        # This contains the version of the compiler that is used to build the project
        if directory is not None and cxx_compiler is not None:
            c = subprocess.check_output([cxx_compiler, "--version"], cwd=directory)
            c = c.decode(locale.getpreferredencoding()).split("\n", 1)[0]
            info["MODM_BUILD_COMPILER"] = c.strip()
    except:
        pass

    info = {k:v.encode("ascii", errors="replace").decode("ascii")
            for k,v in info.items()}

    return info


# -----------------------------------------------------------------------------
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description='Generate a Info file')
    parser.add_argument(
            "-t", "--type",
            dest="type",
            choices=["build", "git", "git_status"],
            help="What information you want.")
    parser.add_argument(
            "-d", "--directory",
            dest="directory",
            default=Path.cwd(),
            help="Which git repository.")
    parser.add_argument(
            "--compiler",
            default="gcc",
            help="Which compiler for build info")
    parser.add_argument(
            "-o", "--output",
            dest="output",
            default="info.c",
            help="Generated info file.")
    parser.add_argument(
            "--check-rebuild",
            dest="check_rebuild",
            default=False,
            action="store_true",
            help="Check content of generated file before regenerating.")
    parser.add_argument(
            "--additions",
            dest="additions",
            action="append",
            help="Additional information in format: NAME=value")

    args = parser.parse_args()

    if "git" in args.type:
        information = git_info(args.directory, with_status="status" in args.type)
    elif "build" in args.type:
        information = build_info(args.directory, args.compiler)

    if args.additions:
        information.update({v.split("=")[0]: v.split("=")[1] for v in args.additions})
    subs = {"type": args.type.split("_")[0], "defines": information}

    render_output = True
    output = Path(args.output)
    if args.check_rebuild and output.exists():
        content = Path(args.output).read_text()
        content = set(re.findall(r"(\w+?) = (.*);", content))
        infocontent = {(k, str(v) if isinstance(v, int) else '"{}"'.format(v.replace('"', '\\"')))
                        for k,v in information.items()}
        if content == infocontent:
            render_output = False

    if render_output:
        template = Path(TEMPLATE_SOURCE)
        loader = jinja2.FileSystemLoader(template.parent)
        loader = jinja2.Environment(loader=loader)
        template = loader.get_template(template.name)
        output.write_text(template.render(subs))
