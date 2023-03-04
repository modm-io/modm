#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2020, 2023, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

from contextlib import contextmanager

class DebugBackend:
    def __init__(self, port=None):
        self.port = port

    def init(self, elf):
        if self.port is None: return []
        return ["target extended-remote {}".format(self.port)]

    def start(self):
        pass

    def stop(self):
        pass

    @contextmanager
    def scope(self):
        try:
            self.start()
            yield
        finally:
            self.stop()
