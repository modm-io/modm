# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


find ${@:1} -name project.xml.log -delete
find ${@:1} -name SConstruct -delete
find ${@:1} -name Makefile -delete
find ${@:1} -name CMakeLists.txt -delete
find ${@:1} -name openocd.cfg -delete
find ${@:1} -name gdbinit -delete

find ${@:1} -type d -name modm -exec rm -rf "{}" \;
find ${@:1} -type d -name generated -exec rm -rf "{}" \;
find ${@:1} -type d -name build -exec rm -rf "{}" \;