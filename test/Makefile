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
# LBUILD = lbuild
LBUILD = $(shell which lbuild)
# SCONS = python3 `which scons`
SCONS = scons

define compile-test
	@$(RM) -r ../build/generated-unittest/$(1)
	$(LBUILD) -c config/$(1).xml $(3) -p ../build/generated-unittest/$(1) build --no-log
	$(SCONS) -C ../build/generated-unittest/$(1) $(2)
endef
define run-test
	$(call compile-test,$(1),$(2) program,$(3))
endef


run-hosted-linux:
	$(call compile-test,hosted,run,-D":target=hosted-linux")
run-hosted-darwin:
	$(call compile-test,hosted,run,-D":target=hosted-darwin")
run-hosted-windows:
	$(call compile-test,hosted,run,-D":target=hosted-windows")


compile-nucleo-f091:
	$(call compile-test,nucleo-f091,size)
run-nucleo-f091:
	$(call run-test,nucleo-f091,size)


compile-nucleo-f401:
	$(call compile-test,nucleo-f401,size)
run-nucleo-f401:
	$(call run-test,nucleo-f401,size)


compile-nucleo-f411:
	$(call compile-test,nucleo-f411,size)
run-nucleo-f411:
	$(call run-test,nucleo-f411,size)


compile-nucleo-f446:
	$(call compile-test,nucleo-f446,size)
run-nucleo-f446:
	$(call run-test,nucleo-f446,size)


compile-nucleo-f446ze:
	$(call compile-test,nucleo-f446ze,size)
run-nucleo-f446ze:
	$(call run-test,nucleo-f446ze,size)


compile-nucleo-l432:
	$(call compile-test,nucleo-l432,size)
run-nucleo-l432:
	$(call run-test,nucleo-l432,size)


compile-nucleo-g474:
	$(call compile-test,nucleo-g474,size)
run-nucleo-g474:
	$(call run-test,nucleo-g474,size)


compile-nucleo-f103_A:
	$(call compile-test,nucleo-f103_A,size)
run-nucleo-f103_A:
	$(call run-test,nucleo-f103_A,size)

compile-nucleo-f103_B:
	$(call compile-test,nucleo-f103_B,size)
run-nucleo-f103_B:
	$(call run-test,nucleo-f103_B,size)


compile-al-avreb-can:
	$(call compile-test,al-avreb-can,size)
run-al-avreb-can:
	$(call run-test,al-avreb-can,size)


compile-arduino-uno:
	$(call compile-test,arduino-uno,size)
run-arduino-uno:
	$(call run-test,arduino-uno,size)


# Currenly only unittests _A, _C, _D, _E, _G are functional
compile-arduino-nano_A:
	$(call compile-test,arduino-nano_A,size)
run-arduino-nano_A:
	$(call run-test,arduino-nano_A,size)
compile-arduino-nano_B:
	$(call compile-test,arduino-nano_B,size)
run-arduino-nano_B:
	$(call run-test,arduino-nano_B,size)
compile-arduino-nano_C:
	$(call compile-test,arduino-nano_C,size)
run-arduino-nano_C:
	$(call run-test,arduino-nano_C,size)
compile-arduino-nano_D:
	$(call compile-test,arduino-nano_D,size)
run-arduino-nano_D:
	$(call run-test,arduino-nano_D,size)
compile-arduino-nano_E:
	$(call compile-test,arduino-nano_E,size)
run-arduino-nano_E:
	$(call run-test,arduino-nano_E,size)
compile-arduino-nano_F:
	$(call compile-test,arduino-nano_F,size)
run-arduino-nano_F:
	$(call run-test,arduino-nano_F,size)
compile-arduino-nano_G:
	$(call compile-test,arduino-nano_G,size)
run-arduino-nano_G:
	$(call run-test,arduino-nano_G,size)
compile-arduino-nano_H:
	$(call compile-test,arduino-nano_H,size)
run-arduino-nano_H:
	$(call run-test,arduino-nano_H,size)


compile-mega-2560-pro:
	$(call compile-test,mega-2560-pro,size)
run-mega-2560-pro:
	$(call run-test,mega-2560-pro,size)
