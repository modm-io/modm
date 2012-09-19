% Installation Linux


Installation on Linux systems
=============================

For Ubuntu (and other Debian based Linux-Distributions) just type the 
following in a terminal and you're good to go:

	$ sudo apt-get install python scons python-jinja2 python-lxml doxygen graphviz git


PC
--

To compile programs for x86 systems you need the following packets:

	$ sudo apt-get install gcc build-essential tipcutils libboost-thread-dev libboost-system-dev libasio-dev


AVR
---

Some linux distributions offer the AVR toolchain already integrated, but
the supplied tools are sometimes not quite up to date.

There some alternatives to get a recent toolchain:
- <a href="http://avr-libc.nongnu.org/user-manual/install_tools.html">Build the tools by hand</a>
- <a href="http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=95328&start=0">Build-Script on avrfreaks.net</a>
  (with ready-to-use Debian/Ubuntu packages)
- ...

If you're lucky and your distribution offers recent packages the following might do:

	$ sudo apt-get install gcc-avr binutils-avr avr-libc avrdude


Optimisations
-------------

You should tell SCons the location of the xpcc folder, so you can use XPCC without having to provide its location in every project.
To use more than one CPU you might want to add the following to the end of your
'.bashrc' file (adjust the '4' to the number of CPUs in your system):

	export XPCC_HOME="path/to/the/xpcc/root/folder/"
	export SCONSFLAGS="-Q -j4 --site-dir=${XPCC_HOME}/scons/"

For more information on the parameter this read the chapter about
\ref build_system "SCons".


avrdude and USB programmers
---------------------------

When using Ubuntu avrdude might not detect USB programmers out of the box. The
problem is that it has not the necessary permission. To allow avrdude to access
USB programmers do the following:

	gksu gedit /etc/udev/rules.d/80-usbprog.rules

Add the following content:

	ATTR{idVendor}=="03eb", ATTR{idProduct}=="2104", GROUP="plugdev", MODE="0660" # AVRISP mkII
	ATTR{idVendor}=="03eb", ATTR{idProduct}=="2107", GROUP="plugdev", MODE="0660" # AVR-Dragon
	ATTR{idVendor}=="03eb", ATTR{idProduct}=="2103", GROUP="plugdev", MODE="0660" # JTAG ICE mkII
	ATTR{idVendor}=="03eb", ATTR{idProduct}=="2106", GROUP="plugdev", MODE="0660" # STK600
	ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", GROUP="plugdev", MODE="0660" # USBASP von www.fischl.de
	ATTR{idVendor}=="03eb", ATTR{idProduct}=="2ffa", GROUP="plugdev", MODE="0660" # AT90USB
	ATTR{idVendor}=="10c4", ATTR{idProduct}=="ea60", GROUP="plugdev", MODE="0660" # AVR910

Now *udev* has to be restarted:

	sudo /etc/init.d/udev restart

If the programmer was connected you have to disconnect and reconnect. After
this everything should work.
