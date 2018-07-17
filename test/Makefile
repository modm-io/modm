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
LBUILD = lbuild

define compile-test
	@$(RM) -r ../build/unittest/$(1)/modm
	$(LBUILD) -c $(1).xml $(3) -p ../build/unittest/$(1) build --no-log
	scons -C ../build/unittest/$(1) $(2)
endef
define run-test
	$(call compile-test,$(1),$(2) program,$(3))
endef


run-hosted-linux:
	$(call compile-test,hosted,run,-D":target=hosted-linux")
run-hosted-darwin:
	$(call compile-test,hosted,run,-D":target=hosted-darwin")


compile-nucleo-f401:
	$(call compile-test,nucleo-f401,size)
run-nucleo-f401:
	$(call run-test,nucleo-f401,size)


compile-nucleo-f411:
	$(call compile-test,nucleo-f411,size)
run-nucleo-f411:
	$(call run-test,nucleo-f411,size)


compile-nucleo-f103:
	$(call compile-test,nucleo-f103,size)
run-nucleo-f103:
	$(call run-test,nucleo-f103,size)


compile-al-avreb-can:
	$(call compile-test,al-avreb-can)
run-al-avreb-can:
	$(call run-test,al-avreb-can)