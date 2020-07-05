# Installation

This is the required software for generating, compiling and programming projects
with modm:

- [Python 3](http://www.python.org/)
- [Software Construct][scons] or [CMake][]
- [Library Builder][lbuild]
- AVR toolchain: [avr-gcc][] and [avrdude][]
- ARM toolchain: [gcc-arm-toolchain][] and [OpenOCD][] (HEAD version!).
- Optional: [Doxygen][] or [Doxypress][]
- Optional: [gdbgui][] for IDE-independent debugging

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

To program Microchip SAM devices via the bootloader, install the `bossac` tool:

	brew cask install bossa

We recommend the use of a graphical frontend for GDB called [gdbgui][]:

	pip3 install gdbgui

SCons now works with Python 3. Unfortunately, macOS still defaults to Python 2.
For a less intrusive way to run all scons scripts with Python 3 add this to your
`.bashrc` or `.zshrc`:

	alias scons="/usr/bin/env python3 $(which scons)"
	# or if your using scons elsewhere too:
	alias scons3="/usr/bin/env python3 $(which scons)"

To compile modm *for macOS* (and not the embedded target) you need to install
some of these libraries as well, depending on what modm modules you use:

	brew install boost gcc


## Linux

For Ubuntu 20.04LTS, these commands install the basic build system:

	sudo apt-get install python3 python3-pip scons git
	sudo apt-get --no-install-recommends install doxygen
	pip3 install modm

!!! warning "Use Python 3!"
	Ubuntu 18.04LTS unfortunately defaults to Python 2, however, our SCons tools
	require Python 3. For a less intrusive way to run all scons scripts with
	Python 3 add this to your `.bashrc` or `.zshrc`:

		alias scons="/usr/bin/env python3 $(which scons)"

	If you get errors about missing `pyelftools`, check that you're really
	running SCons with Python 3!

Install the AVR toochain:

	sudo apt-get install gcc-avr binutils-avr avr-libc avrdude

!!! bug "avr-gcc on Ubuntu"
	Ubuntu does not provide an up-to-date version of avr-gcc that supports C++17.
	For our CI we've created a [precompiled version of avr-gcc for Ubuntu][avr-gcc-latest].
	Unfortunately its path is hardcoded to `/work/avr-gcc`.

Install the ARM toochain by downloading [the pre-built version][gcc-arm-toolchain]
for 64-bit Linux and adding its `/bin` directory to your path.
**Even though your distribution may ship their own ARM toolchain, we very strongly
recommend using the official toolchain, since all of modm is tested with it.**

Install OpenOCD via your package manager:

	sudo apt-get install openocd

The latest OpenOCD release v0.10.0 (as of May 2020) is too old for some targets
(STM32G0, STM32G4, STM32F7). To program these targets you need to compile the
[HEAD version of OpenOCD from source][openocd-source].

We recommend the use of a graphical frontend for GDB called [gdbgui][]:

	pip3 install gdbgui

To compile modm *for Linux* (and not the embedded target) you need to install
some of these libraries as well, depending on what modm modules you use:

	sudo apt-get install gcc build-essential libboost-all-dev


## Windows

We will use [Anaconda][] ([Miniconda Windows installation][miniconda] is
sufficient) to create a new Python 3 environment and install all necessary
packages:

    conda create --name modm python=3 pip
    activate modm
    conda install -c conda-forge git
    pip install jinja2 scons future pyelftools lbuild gdbgui

For ARM development install the Windows 32-bit build of the [GNU Arm Embedded
Toolchain][gcc-arm-toolchain]. For programming and debugging ARM Cortex-M
devices install the pre-build [OpenOCD binaries][openocd_binaries].
You'll need to add both `/bin` paths to your `PATH` variable manually.

!!! warning "For non-English speakers"
	For now project and build paths containing non-ASCII characters are not
	parsed correctly.

!!! warning "Windows paths"
	Windows created maximal incompatibility with it's `\` path separator.
	Even though we try hard to not hardcode the path separator, there may still
	be issues related to this. [Please open an issue][newissue] in that case.

!!! info "Dear Windows users"
	We don't regularly use Windows with modm, so we rely on YOU to keep these
	install instructions working and up-to-date. Please [open a PR with
	improvements][contribute].


[contribute]: https://github.com/modm-io/modm/blob/develop/CONTRIBUTING.md
[newissue]: https://github.com/modm-io/modm/issues/new
[examples]: https://github.com/modm-io/modm/tree/develop/examples
[gcc-arm-toolchain]: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
[openocd]: http://openocd.org
[openocd-source]: https://github.com/ntfreak/openocd
[avr-gcc]: https://www.nongnu.org/avr-libc
[avrdude]: https://www.nongnu.org/avrdude
[lbuild]: https://github.com/modm-io/lbuild
[scons]: https://www.scons.org
[cmake]: https://www.cmake.org
[anaconda]: https://www.anaconda.com
[miniconda]: https://docs.conda.io/en/latest/miniconda.html#windows-installers
[avr-gcc-latest]: https://github.com/modm-ext/docker-avr-gcc/releases
[openocd_binaries]: https://gnutoolchains.com/arm-eabi/openocd
[doxygen]: http://www.doxygen.nl
[doxypress]: https://www.copperspice.com/documentation-doxypress.html
[gdbgui]: https://www.gdbgui.com
