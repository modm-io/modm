#!/bin/zsh

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function ctrl_c() {
    echo "** Trapped CTRL-C"
    exit
}

./cleanup.sh ${@:1}
find ${@:1} -type f -iname "project.xml" -print0 | while IFS= read -r -d $'\0' line; do
	lbuild="$(pwd)/../../library-builder/scripts/lbuild"
    prodir=$(dirname $line)
    echo
    echo "============================================================================================"
    echo "Compiling: $prodir"
	(cd $prodir && $lbuild build && scons -c >/dev/null && scons build >/dev/null; rm -rf ext scons src link tools communication SConstruct SConscript project.xml.log)
done
