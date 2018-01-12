
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