# Installation

This is the required software for generating, compiling and programming projects
with modm:

- [Python 3](http://www.python.org/)
- [Software Construct](http://www.scons.org/)
- [Library Builder][lbuild]
- AVR toolchain: [avr-gcc][] and [avrdude][]
- ARM toolchain: [gcc-arm-toolchain][] and [OpenOCD][].
- [Doxygen](http://www.stack.nl/~dimitri/doxygen)

Note that the modm examples use the SCons build system by default, however,
you are not *required* to use it. See [the reference manual](../reference) for
additional build system documentation.

!!! info "Use GCC 7 or newer"
	modm uses C++17, so you need *at least* GCC 7.

!!! warning "Beware of AVRs"
	We **strongly discourage** using AVRs for new designs, due to a significant
	lack of commitment from Atmel on keeping their AVR toolchain up-to-date.

!!! tip "Check your Locale"
	If you get a `UnicodeEncodeError` when calling `lbuild`, you may need to add
	`export LC_ALL=C.UTF-8` to your `.bashrc`. Consult the internet until
	`python3 -c "import sys; print(sys.stdout.encoding)"` returns `UTF-8`.

Please help us [keep these instructions up-to-date][contribute]!


## macOS

We will use [Homebrew](http://brew.sh/) to install the minimal build system:

	brew install python3 scons git doxygen
	pip3 install --user lbuild jinja2 pyelftools
	pip install --user pathlib pyelftools

Install the [AVR toolchain from source](https://github.com/osx-cross/homebrew-avr):

	brew tap osx-cross/avr
	brew install avr-gcc

And the [pre-built ARM toolchain](https://github.com/osx-cross/homebrew-arm) as
well:

	brew tap osx-cross/arm
	brew install arm-gcc-bin
	brew install openocd --HEAD

To compile modm for macOS you need to install these libraries:

	brew install boost gtkmm sdl zmqpp gcc


## Linux

For Ubuntu 18.04LTS, these commands install the basic build system:

	sudo apt-get install python3 python3-pip scons git
	sudo apt-get --no-install-recommends install doxygen
	pip3 install --user lbuild jinja2 pyelftools
	pip install --user pathlib pyelftools

Install the AVR toochain:

	sudo apt-get install gcc-avr binutils-avr avr-libc avrdude

!!! bug "avr-gcc on Ubuntu"
	Ubuntu does not provide a more recent avr-gcc than v4.9, which does not
	support C++17 (not even C++14). For our CI we've created a [precompiled
	version of avr-gcc 7 for Ubuntu](https://github.com/salkinium/docker-avr-gcc-7).
	Use at your own risk.

And the official ARM toolchain as well:

	sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa
	sudo apt-get update
	sudo apt-get install gcc-arm-embedded openocd

!!! tip "Manual installation on 64-bit Linux"
	You can also download [a precompiled version of the toolchain for 64-bit
	Linux][gcc-arm-toolchain] and manually add the `/bin` directory to your path.
	Use this if your distro does not provide you with an up-to-date ARM toolchain.

To compile modm for Linux you need to install these packages:

	sudo apt-get install gcc build-essential libboost-all-dev libzmqpp-dev \
						 libsdl1.2-dev libsdl-image1.2-dev libgtkmm-2.4-dev


## Windows

We will use Anaconda to create a new Python 3 environment and install all
necessary packages:

    conda create --name modm python=3 pip
    activate modm
    conda install -c conda-forge git
    pip install jinja2 scons future pyelftools lbuild

For ARM development install the Windows 32-bit build of the [GNU Arm Embedded
Toolchain][gcc-arm-toolchain]. For programming and debugging ARM Cortex-M
devices install the pre-build [OpenOCD binaries](http://gnutoolchains.com/arm-eabi/openocd/).
You'll need to add both `/bin` paths to your `PATH` variable manually.

!!! warning "For non-English speakers"
	For now project and build paths containing non-ASCII characters are not parsed correctly.

!!! info "Dear Windows users"
	We don't regularly use Windows with modm, so we rely on YOU to keep these install instructions
	working and up-to-date. Please [open a PR with improvements][contribute].


[contribute]: https://github.com/modm-io/modm/blob/develop/CONTRIBUTING.md
[examples]: https://github.com/modm-io/modm/tree/develop/examples
[gcc-arm-toolchain]: https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads
[openocd]: http://openocd.org
[avr-gcc]: http://www.nongnu.org/avr-libc
[avrdude]: http://www.nongnu.org/avrdude
[lbuild]: https://github.com/modm-io/lbuild
