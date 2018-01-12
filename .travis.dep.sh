# Preparing the cache
if [ ! -f "$HOME/cache/avr-gcc.tar.bz2" ]; then
    echo "Downloading AVR toolchain..."
    (cd $HOME/cache && wget -q http://box.xpcc.io/avr-gcc.tar.bz2) &
fi
if [ ! -f "$HOME/cache/cortex-m.tar.bz2" ]; then
    echo "Downloading Cortex-M toolchain..."
    (cd $HOME/cache && wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2 -O cortex-m.tar.bz2) &
fi
if [ ! -f "$HOME/cache/boost.tar.bz2" ]; then
    echo "Downloading libboost package..."
    (cd $HOME/cache && wget -q http://box.xpcc.io/boost.tar.bz2) &
fi
# wait for all downloads to finish
wait
echo "Downloads done."

# unzip all toolchains
mkdir $HOME/toolchain
echo "Expanding AVR toolchain..."
(cd $HOME/toolchain && tar -xjf $HOME/cache/avr-gcc.tar.bz2) &
echo "Expanding Cortex-M toolchain..."
(cd $HOME/toolchain && tar -xjf $HOME/cache/cortex-m.tar.bz2) &
echo "Expanding boost package..."
(cd $HOME/toolchain && tar -xjf $HOME/cache/boost.tar.bz2) &
# synchronize
wait
echo "Expanding done."