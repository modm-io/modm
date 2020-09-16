# Installation

This is the required software for generating, compiling and programming projects
with modm:

- [Python 3](http://www.python.org/)
- [Software Construct][scons] or [CMake][]
- [Library Builder][lbuild]
- AVR toolchain: [avr-gcc][] and [avrdude][]
- ARM toolchain: [gcc-arm-toolchain][] and [OpenOCD][] (HEAD version!).
- Optional: [Doxypress][] (preferred) or [Doxygen][]
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


## Linux

For Ubuntu 20.04LTS, these commands install the basic build system:

	sudo apt install python3 python3-pip scons cmake git doxygen
	pip3 install modm gdbgui

!!! info "Python Packages in PATH"
	The pip3 command installs executables into `~/.local/bin`, which
	must be added to PATH if it is not already the case.
	Add the following line to the end of your `~/.bashrc` file:

		export PATH="~/.local/bin:$PATH"

For host system (x86_64/arm64) targets:

	sudo apt install gcc build-essential libboost-all-dev

For ARM Cortex-M targets:

	sudo apt install openocd

!!! warning "Ubuntus 'gcc-arm-none-eabi' package"
	The Ubuntu package 'gcc-arm-none-eabi' causes [issues at the moment](https://github.com/modm-io/modm/issues/468).
	We recommend using the toolchain provided by ARM:

		wget -O- https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2 | sudo tar xj -C /opt/
	
	Add it to your systems PATH variable by appending the following line to `~/.bashrc`:

		export PATH="/opt/gcc-arm-none-eabi-9-2020-q2-update/bin:$PATH"

For Atmel/Microchip AVR targets:

	wget -O- https://github.com/modm-ext/docker-avr-gcc/releases/download/v10.2.0/avr-gcc.tar.bz2 | sudo tar xj -C /opt

Add it to your systems PATH variable by appending the following line to `~/.bashrc`:

	export PATH="/opt/avr-gcc/avr-gcc/bin:/opt/avr-gcc/avr-binutils/bin:$PATH"

We recommend Doxypress to generate the documentation:
Download the [binary][doxypress_binaries] for your distribution and unzip it:

	sudo mkdir /opt/doxypress
	wget -O- https://download.copperspice.com/doxypress/binary/doxypress-1.4.0-ubuntu20.04-x64.tar.bz2 | sudo tar xj -C /opt/doxypress

... and add the directory to your `PATH` variable by appending the following line
to `~/.bashrc`:

	export PATH="/opt/doxypress:$PATH"

!!! warning "OpenOCD for recent targets"
	The latest OpenOCD release v0.10.0 (as of May 2020) is too old for some targets
	(STM32G0, STM32G4, STM32F7). To program these targets you need to compile the
	[HEAD version of OpenOCD from source][openocd-source], install it and add it to
	the beginning of your `$PATH`.
	[Here](https://rleh.de/2019/10/08/openocd-stm32-stm32g4-stm32g0.html) is a guide
	on how to do it.

!!! warning "Ubuntu ≤ 19.10"
	Ubuntu ≤ 19.10 unfortunately defaults to Python 2, however, our SCons tools
	require Python 3. For a less intrusive way to run all scons scripts with
	Python 3 add this to your `.bashrc` or `.zshrc`:

		alias scons="/usr/bin/env python3 $(which scons)"

	If you get errors about missing `pyelftools`, check that you're really
	running SCons with Python 3!


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

We recommend using a graphical frontend for GDB called [gdbgui][]:

	pip3 install gdbgui

To compile modm *for x86_64 macOS* you need to install these tools too:

	brew install boost gcc

We recommend using Doxypress to generate the API documentation:

	brew tap modm-ext/modm
	brew install doxypress


## Windows

We will use [Anaconda][] ([Miniconda Windows installation][miniconda] is
sufficient) to create a new Python 3 environment and install all necessary
packages:

    conda create --name modm python=3 pip
    activate modm
    conda install -c conda-forge git
    pip install modm scons future gdbgui

For ARM development install the Windows 32-bit build of the [GNU Arm Embedded
Toolchain][gcc-arm-toolchain]. For programming and debugging ARM Cortex-M
devices install the pre-build [OpenOCD binaries][openocd_binaries].
You need to add both `/bin` paths to your `PATH` variable manually.

We recommend using Doxypress to generate the documentation:
Download the [Installer (*.exe)][doxypress_binaries] and run it.

!!! warning "For non-English speakers"
	For now project and build paths containing non-ASCII characters are not
	parsed correctly.

!!! warning "Windows paths"
	Windows created maximal incompatibility with its `\` path separator.
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
[doxypress_binaries]: https://download.copperspice.com/doxypress/binary/
[gdbgui]: https://www.gdbgui.com
