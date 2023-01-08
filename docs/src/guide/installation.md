# Installation

This is the required software for generating, compiling and programming projects
with modm:

- [Python 3](http://www.python.org/).
- [Software Construct][scons] or [CMake][].
- [Library Builder][lbuild].
- AVR toolchain: [avr-gcc][] and [avrdude][].
- ARM toolchain: [gcc-arm-toolchain][] and [OpenOCD][] (at least v0.11!).
- Optional: [Doxypress][].
- Optional: [gdbgui][] for IDE-independent debugging.

Note that the modm examples use the SCons build system by default, however,
you are not *required* to use it. See [the reference manual](../../reference/build-systems)
for additional build system documentation.

!!! info "Use GCC 12 or newer"
    modm uses C++20, so you need *at least* GCC 10. However, we recommend using GCC12.

!!! warning "Beware of AVRs"
    We **strongly discourage** using AVRs for new designs, due to a significant
    lack of commitment from Atmel on keeping their AVR toolchain up-to-date.

!!! tip "Check your Locale"
    If you get a `UnicodeEncodeError` when calling `lbuild`, you may need to add
    `export LC_ALL=C.UTF-8` to your `.bashrc`. Consult the internet until
    `python3 -c "import sys; print(sys.stdout.encoding)"` returns `UTF-8`.

Please help us [keep these instructions up-to-date][contribute]!


## Linux

For Ubuntu 22.04LTS, these commands install the minimal build system:

```sh
sudo apt install python3 python3-pip scons git libncurses5
pip3 install modm
```

We recommend using a graphical frontend for GDB called [gdbgui][]:

```sh
pip3 install gdbgui
```

!!! info "Python Packages in PATH"
    The pip3 command installs executables into `~/.local/bin`, which
    must be added to PATH if it is not already the case.
    Add the following line to the end of your `~/.bashrc` file:

    ```sh
    export PATH="$HOME/.local/bin:$PATH"
    ```

!!! warning "Ubuntu ≤ 19.10"
    Ubuntu ≤ 19.10 unfortunately defaults to Python 2, however, our SCons tools
    require Python 3. For a less intrusive way to run all scons scripts with
    Python 3 add this to your `.bashrc` or `.zshrc`:

    ```sh
    alias scons="/usr/bin/env python3 $(which scons)"
    ```

    If you get errors about missing `pyelftools`, check that you're really
    running SCons with Python 3!

We use [Doxypress][doxypress_binaries] to generate the API documentation:

```sh
sudo mkdir /opt/doxypress
wget -O- https://github.com/copperspice/doxypress/releases/download/dp-1.5.0/doxypress-1.5.0-ubuntu22.04-x64.tar.bz2 | sudo tar xj -C /opt/doxypress
```

Add the directory to your `PATH` variable in `~/.bashrc`:

```sh
export PATH="/opt/doxypress:$PATH"
```


#### ARM Cortex-M

Install the [pre-built ARM toolchain][gcc-arm-toolchain]:

```sh
wget -O- https://developer.arm.com/-/media/Files/downloads/gnu/12.2.rel1/binrel/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi.tar.xz | sudo tar xj -C /opt/
```

Add it to your `PATH` variable in `~/.bashrc`:

```sh
export PATH="/opt/arm-gnu-toolchain-12.2.rel1-x86_64-arm-none-eabi/bin:$PATH"
```

!!! warning "Ubuntus 'gcc-arm-none-eabi' package"
    The Ubuntu package 'gcc-arm-none-eabi' can [cause issues][armgcc-issues],
    we recommend using only the pre-built toolchain.

Install the OpenOCD tool:

```sh
sudo apt install openocd
```

!!! warning "OpenOCD < v0.11"
    Make sure to get at least OpenOCD release v0.11, since v0.10 is too old for
    some targets (STM32G0, STM32G4, STM32F7). You can manually install an
    up-to-date version of OpenOCD by [following the instructions here][openocd-install].


#### Microchip AVR

Download and extract the [pre-built AVR toolchain][modm-avr-gcc]:

```sh
wget -O- https://github.com/modm-io/avr-gcc/releases/download/v12.2.0/modm-avr-gcc.tar.bz2 | sudo tar xj -C /opt
```

!!! warning "AVR toolchain install directory"
    It is unfortunately not possible to install the AVR toolchain into a
    directory other than directly into `/opt`.

Add the `bin` directory to your `PATH` variable in `~/.bashrc`:

```sh
export PATH="/opt/avr-gcc/bin:$PATH"
```

Install the AvrDude tool:

```sh
sudo apt install avrdude
```


#### Hosted

To compile modm for host system targets (x86_64/arm64):

```sh
sudo apt install gcc build-essential libboost-all-dev
```


## macOS

Install the minimal build system via [Homebrew](http://brew.sh):

```sh
brew update
brew install python3 scons git doxygen
pip3 install modm
```

We recommend using a graphical frontend for GDB called [gdbgui][]:

```sh
pip3 install gdbgui
```

We use [Doxypress][] to generate the API documentation:

```sh
brew tap modm-ext/modm
brew install doxypress
```


#### ARM Cortex-M

Install the [pre-built ARM toolchain](https://github.com/osx-cross/homebrew-arm):

```sh
brew tap osx-cross/arm
brew install arm-gcc-bin@12
brew install openocd --HEAD
```

To program Microchip SAM devices via the bootloader, install the `bossac` tool:

```sh
brew cask install bossa
```


#### Microchip AVR

Install the [AVR toolchain from source](https://github.com/osx-cross/homebrew-avr):

```sh
brew tap osx-cross/avr
brew install avr-gcc@12
brew link --force avr-gcc@12
```


#### Hosted

To compile modm *for x86_64 macOS* you need to install these tools too:

```sh
brew install boost gcc
```


## Windows

In general, Windows is not a great fit for the command line tools that modm is
built on. Consider using the [Windows subsystem for Linux 2 (WSL2)](#windows-with-wsl2)
instead.
Alternatively consider using a better terminal emulator than command prompt
(`cmd`) or PowerShell, perhaps use the official [Windows Terminal][winterm].

!!! warning "Debugging with `arm-none-eabi-gdb`"
    We could not get `arm-none-eabi-gdb` to work on Windows, it seems not to
    support TUI mode and the web-based GDBGUI does not support Windows either.
    If you find a solution, please [open a PR so we can fix this][contribute].

These instructions assume a vanilla Windows 10 installation.

Install [Git via the official installer][wingit]. Make sure to **unselect vim**
as the default Git editor and instead choose at least the Wordpad editor or better.

Install [Python3 via the full installer][pydl]. Make sure to select **Add Python
to PATH** during installation!

Test the installation by opening the command prompt (`cmd`):

```sh
python -c "print('Hello World')"
```

Then install the necessary Python packages:

```sh
pip install modm scons
```

Test that you have access to the `lbuild` and `scons` tools inside the command prompt:

```sh
lbuild --version
scons --version
```

We use [Doxypress][] to generate the API documentation.
Download and run the [Installer][doxypress_binaries].

Please use the free and open-source [7-Zip file archiver][7_zip] to extract the
files in the next steps.

#### ARM Cortex-M

Install the [pre-built ARM toolchain via the 64-bit installer][gcc-arm-toolchain]
and make sure you select **Add path to environment variable** at the end!
Open a new command prompt to test the compiler:

```sh
arm-none-eabi-gcc --version
```

Install the  and then download the latest [pre-built OpenOCD tool][openocd_binaries]:

1. unpack the `.tar.gz` file using the context menu `7-Zip > Extract Here`.
2. unpack the `.tar` file using `7-Zip > Extract to "openocd-v0.11.0-..."`

Then rename and move the extracted folder to `C:\Program Files (x86)\openocd`.
Open PowerShell to add the `\bin` folder to the `Path`:

```
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\Program Files (x86)\openocd\bin", "User")
```

Close the PowerShell and open a new command prompt to test openocd:

```sh
openocd --version
```

#### Microchip AVR

Download the [pre-built AVR toolchain][winavr] and unpack the `.zip` file using
the context menu `7-Zip > Extract to "avr-gcc-12.1.0-..."`
Then rename and move the extracted folder to `C:\Program Files\avr-gcc`.
Open PowerShell to add the `\bin` folder to the `Path`:

```
[Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\Program Files\avr-gcc\bin", "User")
```

Close the PowerShell and open a new command prompt to test avr-gcc and avrdude:

```sh
avr-gcc --version
avrdude --version
```


!!! warning "For non-English speakers"
    For now project and build paths containing non-ASCII characters are not
    parsed correctly. [Please open an issue][newissue] if this is a problem.

!!! warning "Windows paths"
    Windows created several compatibility issues with its `\` path separator.
    Even though we try hard to not hardcode the path separator, there may still
    be issues related to this. [Please open an issue][newissue] in that case.

!!! info "Dear Windows users"
    We don't regularly use Windows with modm, so please [give us some feedback][newissue]
    about the quality of these instructions.


## Windows with WSL2

The [Windows Subsystem for Linux 2][wsl2] allows you to run a Linux distribution
in parallel to Windows. For a pure Windows installation see [above](#windows).

Install Ubuntu-22.04 LTS as WSL2 distribution:
See the [general WSL doc][wsl-install], but as of this writing
`wsl --list --online` will not list Ubuntu 22.04.1 LTS, but it is
[available in the Microsoft Store][windows-store-ubuntu-22-04-1-lts].
Install it from there via mouse clicks.
Make sure the WSL2 instance is running by opening a terminal via start menu.

Physically attach your microcontroller development board or your debugger to a
USB port and [bridge the USB hub to which the device is attached][wsl-connect-usb].
Also [bridge the USB device to Linux][connect-usb] using [usbipd][usbipd]:

```powershell
PS C:\Windows\system32> usbipd wsl list
BUSID  VID:PID    DEVICE                                STATE
3-2    1bcf:0005  USB-Eingabegerät                      Not attached
3-3    0483:374b  ST-Link Debug, USB-...                Attached - Ubuntu-22.04
4-1    138a:003d  Synaptics FP Sensors (WBF) (PID=003d) Not attached
4-3    04f2:b370  HP HD Webcam [Fixed]                  Not attached
```

Check the BUSID parameter and use the one that corresponds to your device in
further commands. In this example it is the `3-3`.

```powershell
PS C:\Windows\system32> usbipd wsl attach --busid 3-3
usbipd: info: Using default distribution 'Ubuntu-22.04'.
```

!!! tip "Unreliable connection"
    If the connection is not reliable, it might be necessary to detach/attach
    several times until the connection is established:

    ```powershell
    PS C:\Windows\system32> usbipd wsl detach --busid 3-3
    PS C:\Windows\system32> usbipd wsl attach --busid 3-3
    usbipd: info: Using default distribution 'Ubuntu-22.04'.
    ```

Either reboot the machine or be lucky that a combination of the following
commands is sufficient:

```sh
sudo udevadm trigger
sudo udevadm control --reload
```

Remove the USB connector, reattach it, then issue the command
`usbipd wsl attach --busid 3-3`.

On WSL2 Linux follow the [Linux installation instructions](#linux) and
additionally install a terminal emulator such as _picocom_:

```sh
sudo apt install picocom
```

[Explore the examples][examples] and compile and upload a suitable one with
`scons program`. Try to debug it with GDB using `scons debug` and listen on
the serial output:

```sh
picocom --baud 115200 --imap lfcrlf --echo /dev/ttyACM0
```

!!! note "VSCode support"
    You can also install the [remote extensions for WSL][wsl-vscode] and use the
    [VSCode integrated terminal][wsl-vscode-integrated-terminal] to access WSL2.


[connect-usb]: https://docs.microsoft.com/en-us/windows/wsl/connect-usb
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
[openocd_binaries]: https://gnutoolchains.com/arm-eabi/openocd/
[doxygen]: http://www.doxygen.nl
[doxypress]: https://www.copperspice.com/documentation-doxypress.html
[doxypress_binaries]: https://download.copperspice.com/doxypress/binary/
[gdbgui]: https://www.gdbgui.com
[pydl]: https://www.python.org/downloads/
[pywin]: https://docs.python.org/3/using/windows.html
[7_zip]: https://www.7-zip.org
[modm-avr-gcc]: https://github.com/modm-io/avr-gcc
[armgcc-issues]: https://github.com/modm-io/modm/issues/468
[openocd-install]: https://github.com/rleh/openocd-build
[udev-rules-openocd]: https://github.com/openocd-org/openocd/blob/master/contrib/60-openocd.rules#L84-L99
[usbipd]: https://github.com/dorssel/usbipd-win
[winavr]: https://blog.zakkemble.net/avr-gcc-builds/
[windows-store-ubuntu-22-04-1-lts]: https://www.microsoft.com/store/productId/9PN20MSR04DW
[wingit]: https://git-scm.com/download/win
[winterm]: https://github.com/Microsoft/Terminal
[winwsl]: https://docs.microsoft.com/en-us/windows/wsl/about
[wsl2]: https://docs.microsoft.com/en-us/windows/wsl/about#what-is-wsl-2    
[wsl-connect-usb]: https://docs.microsoft.com/en-us/windows/wsl/connect-usb
[wsl-install]: https://docs.microsoft.com/en-us/windows/wsl/install
[wsl-vscode]: https://docs.microsoft.com/en-us/windows/wsl/tutorials/wsl-vscode
[wsl-vscode-integrated-terminal]: https://code.visualstudio.com/docs/remote/wsl-tutorial#_integrated-terminal
