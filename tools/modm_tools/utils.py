#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, 2017-2018, Fabian Greif
# Copyright (c) 2018-2020, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

import re
import glob
import platform
import subprocess


def _listify(obj):
    if obj is None:
        return list()
    if isinstance(obj, (list, tuple, set, range)):
        return list(obj)
    if hasattr(obj, "__iter__") and not hasattr(obj, "__getitem__"):
        return list(obj)
    return [obj, ]


def listify(*objs):
    """
    Convert arguments to list if they are not already a list.
    """
    return [l for o in objs for l in _listify(o)]


def listrify(*objs):
    """
    Convert arguments to list of strings.
    """
    return list(map(str, listify(*objs)))


def guess_serial_port(port_hint=None):
    if "Windows" in platform.platform():
        ports = glob.glob('COM[0-9]*')
    elif "Darwin" in platform.system():
        if port_hint == "bossac":
            ports = glob.glob('/dev/tty.usbmodem*')
        else:
            ports = glob.glob('/dev/tty.usb*')
    else:
        if port_hint == 'bmp':
            import os.path
            # Symlink created by BMP udev rules
            # https://github.com/blackmagic-debug/blackmagic/blob/main/driver/99-blackmagic-plugdev.rules
            if os.path.exists('/dev/ttyBmpGdb'):
                ports = ['/dev/ttyBmpGdb']
            elif os.path.exists('/dev/ttyACM0'):
                ports = ['/dev/ttyACM0']
            else:
                ports = [None]
        else:
            ports = glob.glob('/dev/tty[A-Za-z]*')
    return next(iter(ports), None)


def compiler_version(gcc):
    """
    :returns: the compiler version as a single integer with two decimals per
              sub-version. v10.1.2 -> 100102
    """
    output = subprocess.run(gcc + " -dumpversion", shell=True, capture_output=True, text=True)
    version = re.match(r"^(\d+)\.(\d+)\.(\d+)", output.stdout)
    if version:
        compiler_version = (int(version.group(1)) * 10000 +
                            int(version.group(2)) * 100 +
                            int(version.group(3)))
    else:
        # Compiler version could not be detected
        compiler_version = 0

    return compiler_version
