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


