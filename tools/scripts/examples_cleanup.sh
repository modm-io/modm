# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.


find ${@:1} -name project.xml.log -delete
find ${@:1} -name SConstruct -delete
find ${@:1} -name SConscript -delete

find ${@:1} -type d -name ext -exec rm -rf "{}" \;
find ${@:1} -type d -name link -exec rm -r "{}" \;
find ${@:1} -type d -name src -exec rm -r "{}" \;
find ${@:1} -type d -name scons -exec rm -r "{}" \;
find ${@:1} -type d -name tools -exec rm -r "{}" \;
find ${@:1} -type d -name docs -exec rm -r "{}" \;
find ${@:1} -type d -name test -exec rm -r "{}" \;