# Installation

This is the required software for generating, compiling and programming projects
with modm:

- [Python 3](http://www.python.org/)
- [Software Construct](http://www.scons.org/)
- [Library Builder][lbuild]
- AVR toolchain: [avr-gcc][] and [avrdude][]
- ARM toolchain: [gcc-arm-toolchain][] and [OpenOCD][].
- [Doxygen](http://www.doxygen.nl)

Note that the modm examples use the SCons build system by default, however,
you are not *required* to use it. See [the reference manual](../../reference/build-systems) for
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
	pip3 install modm

Install the [AVR toolchain from source](https://github.com/osx-cross/homebrew-avr):

	brew tap osx-cross/avr
	brew install avr-gcc

And the [pre-built ARM toolchain](https://github.com/osx-cross/homebrew-arm) as
well:

	brew tap osx-cross/arm
	brew install arm-gcc-bin
	brew install openocd --HEAD

scons now works with Python 3. Unfortunately, macOS still defaults to Python 2.
For a less intrusive way to run all scons scripts with Python 3 add this to your
`.bashrc` or `.zshrc`:

	alias scons="/usr/bin/env python3 $(which scons)"

To compile modm *for macOS* (and not the embedded target) you need to install
some of these libraries as well, depending on what modm modules you use:

	brew install boost zmqpp gcc


## Linux

For Ubuntu 18.04LTS, these commands install the basic build system:

	sudo apt-get install python3 python3-pip scons git openocd
	sudo apt-get --no-install-recommends install doxygen
	pip3 install modm

scons now works with Python 3. Unfortunately, Ubuntu still defaults to Python 2.
For a less intrusive way to run all scons scripts with Python 3 add this to your
`.bashrc` or `.zshrc`:

	alias scons="/usr/bin/env python3 $(which scons)"

Install the AVR toochain:

	sudo apt-get install gcc-avr binutils-avr avr-libc avrdude

!!! bug "avr-gcc on Ubuntu"
	Ubuntu does not provide an up-to-date version of avr-gcc that supports C++17.
	For our CI we've created a [precompiled version of avr-gcc for Ubuntu][avr-gcc-latest].
	Use at your own risk.

Install the ARM toochain by downloading [the pre-built version][gcc-arm-toolchain]
for 64-bit Linux and adding its `/bin` directory to your path.
Even though your distribution may ship their own ARM toolchain, we *very strongly
recommend* using the official toolchain, since all of modm is tested with it.

To compile modm *for Linux* (and not the embedded target) you need to install
some of these libraries as well, depending on what modm modules you use:

	sudo apt-get install gcc build-essential libboost-all-dev libzmqpp-dev


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
[gcc-arm-toolchain]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
[openocd]: http://openocd.org
[avr-gcc]: http://www.nongnu.org/avr-libc
[avrdude]: http://www.nongnu.org/avrdude
[lbuild]: https://github.com/modm-io/lbuild
[avr-gcc-latest]: https://github.com/salkinium/docker-avr-gcc/releases
