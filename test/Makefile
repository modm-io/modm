# -*- coding: utf-8 -*-
#
# Copyright (c) 2017, Fabian Greif
# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# LBUILD = python3 ../../library-builder/scripts/lbuild
LBUILD = "lbuild"

test-hosted-linux:
	@$(RM) -r ../build/unittest/hosted
	$(LBUILD) -c hosted.xml -D":target=hosted-linux" -p ../build/unittest/hosted -v build --no-log
	scons -C ../build/unittest/hosted run

test-hosted-darwin:
	@$(RM) -r ../build/unittest/hosted
	$(LBUILD) -c hosted.xml -D":target=hosted-darwin" -p ../build/unittest/hosted -v build --no-log
	scons -C ../build/unittest/hosted run

test-stm32:
	@$(RM) -r ../build/unittest/stm32
	$(LBUILD) -c stm32.xml -p ../build/unittest/stm32 -v build --no-log
	scons -C ../build/unittest/stm32 size

test-avr:
	@$(RM) -r ../build/unittest/avr
	$(LBUILD) -c avr.xml -p ../build/unittest/avr -v build --no-log
	scons -C ../build/unittest/avr
