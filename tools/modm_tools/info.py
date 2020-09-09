#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import re
import locale
import getpass
import platform
import subprocess

from pathlib import Path
from git import Repo

def git_status(directory):
    info = {
        "MODM_GIT_MODIFIED": -1,
        "MODM_GIT_ADDED": -1,
        "MODM_GIT_DELETED": -1,
        "MODM_GIT_RENAMED": -1,
        "MODM_GIT_COPIED": -1,
        "MODM_GIT_UNTRACKED": -1,
    }
    try:
        repo = Repo(directory, search_parent_directories=True)
        # File status
        diff = repo.index.diff(None)
        def default(change_type):
            try:
                return len(list(diff.iter_change_type(change_type)))
            except:
                return -1

        info["MODM_GIT_MODIFIED"]  = len(diff)
        info["MODM_GIT_ADDED"]     = default("A")
        info["MODM_GIT_DELETED"]   = default("D")
        info["MODM_GIT_RENAMED"]   = default("R")
        info["MODM_GIT_COPIED"]    = default("C")
        info["MODM_GIT_UNTRACKED"] = len(repo.untracked_files)
    except:
        pass

    return info


def git_info(directory):
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
    try:
        repo = Repo(directory, search_parent_directories=True)
        # Last Commit Values
        commit = repo.commit()
        info["MODM_GIT_SHA"]                      = str(commit.hexsha)
        info["MODM_GIT_SHA_ABBR"]                 = str(commit.hexsha[:10])
        info["MODM_GIT_AUTHOR"]                   = str(commit.author.name)
        info["MODM_GIT_AUTHOR_EMAIL"]             = str(commit.author.email)
        info["MODM_GIT_AUTHOR_DATE"]              = str(commit.authored_datetime)
        info["MODM_GIT_AUTHOR_DATE_TIMESTAMP"]    = str(commit.authored_date)
        info["MODM_GIT_COMMITTER"]                = str(commit.committer.name)
        info["MODM_GIT_COMMITTER_EMAIL"]          = str(commit.committer.email)
        info["MODM_GIT_COMMITTER_DATE"]           = str(commit.committed_datetime)
        info["MODM_GIT_COMMITTER_DATE_TIMESTAMP"] = str(commit.committed_date)
        info["MODM_GIT_SUBJECT"]                  = str(commit.summary)
        # Git Config
        config = repo.config_reader()
        info["MODM_GIT_CONFIG_USER_NAME"]  = str(config.get_value("user", "name", "Unknown"))
        info["MODM_GIT_CONFIG_USER_EMAIL"] = str(config.get_value("user", "email", "Unknown"))
    except:
        pass

    info = {k:v.encode("ascii", errors="replace").decode("ascii")
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

            m = re.match(r"(?P<name>[a-z\-\+]+)[a-zA-Z\(\) ]* (?P<version>\d+\.\d+\.\d+)", c)
            if m: comp = "{0} {1}".format(m.group("name"), m.group("version"))
            else: comp = c
            info["MODM_BUILD_COMPILER"] = comp.strip()
    except:
        pass

    info = {k:v.encode("ascii", errors="replace").decode("ascii")
            for k,v in info.items()}

    return info
