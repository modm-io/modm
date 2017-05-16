# http://stackoverflow.com/questions/26082444/how-to-work-around-travis-cis-4mb-output-limit/26082445#26082445

# Abort on Error
set -e

export PING_SLEEP=30s
export WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export BUILD_OUTPUT=$WORKDIR/build.out

touch $BUILD_OUTPUT

dump_output() {
   echo Tailing the last 500 lines of output:
   tail -500 $BUILD_OUTPUT
}

error_handler() {
  echo ERROR: An error was encountered with the build.
  dump_output
  exit 1
}

# If an error occurs, run our error handler to output a tail of the build
trap 'error_handler' ERR

# Set up a repeating loop to send some output to Travis.

bash -c "while true; do echo \$(date) - building ...; sleep $PING_SLEEP; done" &
PING_LOOP_PID=$!

# Preparing the cache

cd $HOME/cache
if [ ! -d "$HOME/cache/avr8-gnu-toolchain-linux_x86" ]; then
	echo "Downloading AVR toolchain..."
	wget http://www.atmel.com/images/avr8-gnu-toolchain-3.4.5.1522-linux.any.x86.tar.gz -O avr8.tar.gz
	tar -xzf avr8.tar.gz
	rm avr8.tar.gz
fi
if [ ! -d "$HOME/cache/gcc-arm-none-eabi-6-2017-q1-update" ]; then
	echo "Downloading Cortex-M toolchain..."
	wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2 -O cortex_m.tar.bz2
	tar -xjf cortex_m.tar.bz2
	rm cortex_m.tar.bz2
fi
if [ ! -d "$HOME/cache/boost1.54-1.54.0" ]; then
	echo "Downloading and building libboost1.54 package..."
	wget http://box.xpcc.io/libboost1.54-dev-src.tar.gz
	tar -xzf libboost1.54-dev-src.tar.gz
	rm libboost1.54-dev-src.tar.gz
	cd boost1.54-1.54.0
	./bootstrap.sh >> $BUILD_OUTPUT 2>&1
	./b2 >> $BUILD_OUTPUT 2>&1
fi
if [ ! -d "$HOME/cache/boost" ]; then
	echo "Installing libboost1.54 package..."
	mkdir $HOME/cache/boost
	cd $HOME/cache/boost1.54-1.54.0
	./b2 --prefix=$HOME/cache/boost install >> $BUILD_OUTPUT 2>&1
fi

# The build finished without returning an error so dump a tail of the output
dump_output

# nicely terminate the ping output loop
kill $PING_LOOP_PID

cd $TRAVIS_BUILD_DIR
