cd $HOME/cache
if [ ! -d "$HOME/cache/avr8-gnu-toolchain-linux_x86" ]; then
	echo "Downloading AVR toolchain..."
	wget http://www.atmel.com/images/avr8-gnu-toolchain-3.4.5.1522-linux.any.x86.tar.gz -O avr8.tar.gz
	tar -xzf avr8.tar.gz
	rm avr8.tar.gz
fi
if [ ! -d "$HOME/cache/gcc-arm-none-eabi-4_9-2015q3" ]; then
	echo "Downloading Cortex-M toolchain..."
	wget https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update/+download/gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2 -O cortex_m.tar.bz2
	tar -xjf cortex_m.tar.bz2
	rm cortex_m.tar.bz2
fi
if [ ! -d "$HOME/cache/boost1.54-1.54.0" ]; then
	echo "Downloading and building libboost1.54 package..."
	mkdir cd $HOME/cache
	wget http://box.xpcc.io/libboost1.54-dev-src.tar.gz
	tar -xzf libboost1.54-dev-src.tar.gz
	rm libboost1.54-dev-src.tar.gz
	cd boost1.54-1.54.0
	./bootstrap.sh > /dev/null
	./b2 > /dev/null
fi
if [ ! -d "$HOME/cache/boost" ]; then
	echo "Installing libboost1.54 package..."
	mkdir $HOME/cache/boost
	cd $HOME/cache/boost1.54-1.54.0
	./b2 --prefix=$HOME/cache/boost install > /dev/null
fi
cd $TRAVIS_BUILD_DIR
