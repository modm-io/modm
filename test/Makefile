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

# LBUILD = python3 ../../lbuild
LBUILD = $(shell which lbuild)
# SCONS = python3 `which scons`
SCONS = scons

define compile-test
	@$(RM) -r ../build/generated-unittest/$(1)
	$(LBUILD) -p ../build/generated-unittest/$(1) -c config/$(1).xml $(3) \
			  -C ../build/generated-unittest/$(1) build --no-log
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


compile-nucleo-f091rc_A:
	$(call compile-test,nucleo-f091rc_A,size)
run-nucleo-f091rc_A:
	$(call run-test,nucleo-f091rc_A,size)
compile-nucleo-f091rc_B:
	$(call compile-test,nucleo-f091rc_B,size)
run-nucleo-f091rc_B:
	$(call run-test,nucleo-f091rc_B,size)


compile-nucleo-f103rb_A:
	$(call compile-test,nucleo-f103rb_A,size)
run-nucleo-f103rb_A:
	$(call run-test,nucleo-f103rb_A,size)
compile-nucleo-f103rb_B:
	$(call compile-test,nucleo-f103rb_B,size)
run-nucleo-f103rb_B:
	$(call run-test,nucleo-f103rb_B,size)


compile-nucleo-f401re:
	$(call compile-test,nucleo-f401re,size)
run-nucleo-f401re:
	$(call run-test,nucleo-f401re,size)


compile-nucleo-f411re:
	$(call compile-test,nucleo-f411re,size)
run-nucleo-f411re:
	$(call run-test,nucleo-f411re,size)


compile-nucleo-f429zi:
	$(call compile-test,nucleo-f429zi,size)
run-nucleo-f429zi:
	$(call run-test,nucleo-f429zi,size)


compile-nucleo-f446re:
	$(call compile-test,nucleo-f446re,size)
run-nucleo-f446re:
	$(call run-test,nucleo-f446re,size)


compile-nucleo-f446ze:
	$(call compile-test,nucleo-f446ze,size)
run-nucleo-f446ze:
	$(call run-test,nucleo-f446ze,size)


compile-nucleo-f767zi:
	$(call compile-test,nucleo-f767zi,size)
run-nucleo-f767zi:
	$(call run-test,nucleo-f767zi,size)


compile-nucleo-g474re:
	$(call compile-test,nucleo-g474re,size)
run-nucleo-g474re:
	$(call run-test,nucleo-g474re,size)


compile-nucleo-h723zg:
	$(call compile-test,nucleo-h723zg,size)
run-nucleo-h723zg:
	$(call run-test,nucleo-h723zg,size)


compile-nucleo-l432kc:
	$(call compile-test,nucleo-l432kc,size)
run-nucleo-l432kc:
	$(call run-test,nucleo-l432kc,size)


compile-nucleo-l496zg-p:
	$(call compile-test,nucleo-l496zg-p,size)
run-nucleo-l496zg-p:
	$(call run-test,nucleo-l496zg-p,size)


compile-nucleo-l552ze-q:
	$(call compile-test,nucleo-l552ze-q,size)
run-nucleo-l552ze-q:
	$(call run-test,nucleo-l552ze-q,size)


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


compile-mega-2560-pro_A:
	$(call compile-test,mega-2560-pro_A,size)
run-mega-2560-pro_A:
	$(call run-test,mega-2560-pro_A,size)
compile-mega-2560-pro_B:
	$(call compile-test,mega-2560-pro_B,size)
run-mega-2560-pro_B:
	$(call run-test,mega-2560-pro_B,size)
compile-mega-2560-pro_C:
	$(call compile-test,mega-2560-pro_C,size)
run-mega-2560-pro_C:
	$(call run-test,mega-2560-pro_C,size)


compile-samv71-xplained-ultra:
	$(call compile-test,samv71-xplained-ultra,size)
run-samv71-xplained-ultra:
	$(call run-test,samv71-xplained-ultra,size)
