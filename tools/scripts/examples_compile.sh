# Copyright (c) 2018, Niklas Hauser
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
    echo "** Trapped CTRL-C"
    exit 1
}

# lbuild="$(pwd)/../../library-builder/scripts/lbuild"
lbuild="lbuild"
export RESULT=0

for line in $(find ${@:1} -type f -iname "project.xml"); do
    prodir=$(dirname $line)
    echo
    echo "============================================================================================"
    echo "Compiling: $prodir"
	(cd $prodir && $lbuild build && scons -c >/dev/null && scons build >/dev/null)
    RESULT=`expr $RESULT + $?`
    (cd $prodir && rm -rf ext scons src link tools communication SConstruct SConscript project.xml.log)
done

exit $RESULT


