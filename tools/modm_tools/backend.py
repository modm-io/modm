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

class Empty:
    def init(self, elf):
        return None
    def start(self):
        pass
    def stop(self):
        pass

class ExtendedRemote:
    def __init__(self, host=None, port=None):
        self.host = "localhost" if host is None else host
        self.port = "3333" if port is None else port
    def init(self, elf):
        return ["target extended-remote {}:{}".format(self.host, self.port)]
    def start(self):
        pass
    def stop(self):
        pass

class Scope:
    def __init__(self, backend):
        self.backend = backend
    def __enter__(self):
        self.backend.start()
    def __exit__(self, type, value, traceback):
        self.backend.stop()
