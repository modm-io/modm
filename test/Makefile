#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, Fabian Greif
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

#TMPDIR := $(shell mktemp -d)
TMPDIR := /tmp/unittest

LBUILD = python3 ../../library-builder/scripts/lbuild

test:
	@$(RM) -r $(TMPDIR)/*
	$(LBUILD) -c project.xml -p $(TMPDIR) -v build --module=:test:architecture
	scons -C $(TMPDIR)/test
	@$(TMPDIR)/build/test/unittest
