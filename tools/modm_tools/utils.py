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

import glob
import platform

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
        ports = glob.glob('/dev/tty[A-Za-z]*')
    return next(iter(ports), None)

