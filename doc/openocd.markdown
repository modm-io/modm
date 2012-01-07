
git clone git://openocd.git.sourceforge.net/gitroot/openocd/openocd
cd openocd

// see README
./bootstrap
./configure --prefix=/opt/openocd --enable-ft2232_libftdi --enable-amtjtagaccel --enable-arm-jtag-ew --enable-jlink
make
sudo make install

