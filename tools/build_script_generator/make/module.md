# Makefile Build Script Generator

[Make][] is a simple dependency tracking build system. It's very lightweight and
available pretty much everywhere.

This module generates a set of self contained Makefiles for building modm
applications:

- a `modm/config.mk` file: configures the toolchain, device-specific information
  and sets up generic and special build rules based on the `modm:build` options.
- a `modm/repo.mk` file: contains all build rules to build the modm library.
- a top-level BSD-licensed `Makefile` file: globs the application sources and
  provides all the Makefile targets for the device-specific embedded tools.

We do not intend to serve every possible use-case with this module.
**If you need something special, write your own Makefile, maybe starting by
modifying ours.** It is intentionally BSD-licensed so that you do not have to
publish your changes to it.

Remember to set `modm:build:make:include_makefile` to `False`, so that your
custom `Makefile` does not get overwritten by `lbuild build`.

!!! bug "No Windows Support"
    Due to issues with the Windows path separator `\` the generated Makefile may
    not work correctly on Windows. We recommend using SCons on Windows instead.


## Makefile Targets

This module generates these [`.PHONY` targets][phony].


#### make

Defaults to **make build size**.

You can add these arguments to any of the Make commands:

- `-j8`: Process 8 jobs in parallel. You can also `export MAKEFLAGS="-j8"` in
         your `.bashrc` to have a permanent setting.
- `-n`: gives a verbose, simulated output, so you can check what options the
        compiler is called with.
- `profile=release`: Compile project with the release profile options (default).
- `profile=debug`: Compile project with the debug profile options.

For a description of the `release` and `debug` profiles, see the `modm:build`
module documentation.

!!! tip "Debug Profile"
    When working with the debug profile, make sure to add `profile=debug` to all
    commands, especially `make program profile=debug` and
    `make debug profile=debug`!


#### make build

```
make build profile={debug|release}
```

Compiles your application into an executable.

Example for an embedded target:

```
 $ make build
Compiling C++·· {debug|release}/main.o
Compiling C···· {debug|release}/modm/ext/gcc/cabi.o
    ...
Compiling C++·· {debug|release}/modm/src/modm/utils/dummy.o
Archiving······ {debug|release}/modm/libmodm.a
Indexing······· {debug|release}/modm/libmodm.a
Linking········ /build/{debug|release}/blink.elf
```


#### make clean

```
make clean profile={debug|release}
```

Cleans the build artifacts.


```
 $ make clean
Removing······· {debug|release}
```


#### make run

Compiles and executes your program on your computer.
(\* *only Hosted targets*)


#### make size

```
make size profile={debug|release}
```

Displays the static Flash and RAM consumption of your target.

Example for a STM32 target with 16MB external heap:

```
 $ make size
Memory usage··· /build/{debug|release}/blink.elf
Program:  12.8 KiB (0.6% used)
(.data + .fastcode + .fastdata + .hardware_init + .reset + .rodata +
 .table.copy.extern + .table.copy.intern + .table.section_heap +
 .table.zero.intern + .text)

Data:      5.8 KiB (1.5% used) = 2936 B static (0.7%) + 3040 B stack (0.8%)
(.bss + .data + .fastdata + .stack)

Heap:     16.4 MiB
(.heap0 + .heap1 + .heap2 + .heap3 + .heap5 + .heap_extern)
```


#### make program

```
make program profile={debug|release} [port={serial-port}]
```

Writes the executable onto your target via Avrdude or OpenOCD.
This is a convenience wrapper around the programming options and methods
defined in the `modm:build` module.
(\* *only AVR and ARM Cortex-M targets*)

Example for a STM32 target:

```
 $ make program
Open On-Chip Debugger 0.10.0
    ...
Info : using stlink api v2
Info : Target voltage: 3.259396
Info : stm32f4x.cpu: hardware has 6 breakpoints, 4 watchpoints
    ...
** Programming Started **
auto erase enabled
Info : device id = 0x10006434
Info : flash size = 2048kbytes
Info : Dual Bank 2048 kiB STM32F42x/43x/469/479 found
    ...
wrote 16384 bytes from file {debug|release}/blink.elf in 0.589736s (27.131 KiB/s)
** Programming Finished **
** Verify Started **
verified 13064 bytes in 0.296308s (43.056 KiB/s)
** Verified OK **
shutdown command invoked
```


#### make program-fuses

```
make program-fuses profile={debug|release}
```

Writes all fuses onto your target connected to avrdude. See the
`modm:platform:core` module for how to define the fuse values.
(\* *only AVR targets*)


#### make program-dfu

```
make program-dfu profile={debug|release} [delay={seconds}]
```

Writes the executable onto your target via Device Firmware Update (DFU) over USB.
A DFU bootloader is available on many STM32 microcontrollers
and can be accessed by pressing the BOOT0-Button during startup.

Some DFU devices require additional delay to re-enumerate, which you can specify
with the `delay` parameter (default is 5 seconds).
(\* *only ARM Cortex-M targets*)

```
$ make program-dfu
Binary File···· /build/{debug|release}/blink.bin
dfu_stm32_programmer: program /build/{debug|release}/blink.bin
dfu-util 0.9
Opening DFU capable USB device...
ID 0483:df11
Run-time device DFU version 011a
Claiming USB DFU Interface...
Determining device status: state = dfuIDLE, status = 0
dfuIDLE, continuing
DFU mode device DFU version 011a
Device returned transfer size 2048
DfuSe interface name: "Internal Flash  "
Memory segment at 0x08000000   4 x 16384 = 65536 (rew)
Memory segment at 0x08010000   1 x 65536 = 65536 (rew)
Memory segment at 0x08020000   1 x 131072 = 131072 (rew)
Downloading to address = 0x08000000, size = 2060
Download        [                         ]   0%            0 bytes   Poll timeout 100 ms
   Poll timeout 0 ms
 Download from image offset 00000000 to memory 08000000-080007ff, size 2048
   Poll timeout 104 ms
   Poll timeout 0 ms
 Download from image offset 00000800 to memory 08000800-0800080b, size 12
   Poll timeout 104 ms
   Poll timeout 0 ms
File downloaded successfully
   Poll timeout 104 ms
   Poll timeout 0 ms
Transitioning to dfuMANIFEST state
```


#### make program-bmp

```
make program-bmp profile={debug|release} [port={serial-port}]
```

[Black Magic Probe][bmp] is convenient tool to convert cheap USB ST-LINK V2 clones
to a fully functional GDB compatible debug adaptor for ARM Cortex microcontrollers.
GDB can directly communicate with the debug adaptor making debugging easy and
accessible.
(\* *only ARM Cortex-M targets*)

Black Magic Probe creates two serial devices, the first being the GDB interface
and the second a plain serial adaptor for debugging purposes.

```
$ ls -l /dev/tty.usb*
crw-rw-rw-  1 root  wheel   21, 104 Feb 19 09:46 /dev/tty.usbmodemDEADBEEF
crw-rw-rw-  1 root  wheel   21, 106 Feb 19 09:46 /dev/tty.usbmodemDEADBEF1
```

You can let the tool guess the port or explicitly specify it:

```
$ make program-bmp port=/dev/tty.usbmodemDEADBEEF
Remote debugging using /dev/tty.usbmodemDEADBEEF
Target voltage: unknown
Available Targets:
No. Att Driver
 1      STM32F1 medium density
Attaching to Remote target
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0800038e in ?? ()
Loading section .vector_rom, size 0xec lma 0x8000000
[...]
Loading section .table.section_heap, size 0xc lma 0x80013f8
Start address 0x8000e6c, load size 5120
Transfer rate: 10 KB/sec, 365 bytes/write.
Detaching from program: , Remote target
[Inferior 1 (Remote target) detached]
make: done building targets.
```


#### make debug

```
make debug profile={debug|release} ui={tui|web}
```

Launches OpenOCD in the background, then launches GDB in foreground with the
correct executable with text-based or [web-based GDBGUI][gdbgui] UI. When GDB
exits, it stops the OpenOCD process.
(\* *only ARM Cortex-M targets*)

This is just a convenience wrapper for the debug functionality defined in the
`modm:build` module. To use GDBGUI you must have it installed via
`pip install gdbgui`.

!!! tip "Choose the correct profile"
    When debugging, make sure to select the correct compilation profile. The
    firmware and the executable given to GDB have to be the same or you'll see
    GDB translate the program counter to the wrong code locations. When you
    suspect a bug in your firmware, consider that it was most likely compiled
    with the release profile, since that's the default. First try to `make
    debug profile=release`, and if that doesn't help, compile and `make
    program profile=debug` and try `make debug profile=debug` again.


#### make debug-bmp

```
make debug-bmp profile={debug|release} ui={tui|web} port={serial-port}
```

Launches GDB to debug via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### make debug-coredump

```
make debug-coredump profile={debug|release} ui={tui|web} \
                    coredump={path/to/coredump.txt}
```

Launches GDB for post-mortem debugging with the latest firmware using the data
from the `coredump={filepath}` argument.
(\* *only ARM Cortex-M targets*)

See the `modm:platform:fault` module for details how to receive the coredump data.


#### make reset

```
make reset
```

Resets the executable via OpenOCD.
(\* *only ARM Cortex-M targets*)


#### make reset-bmp

```
make reset-bmp [port={serial}]
```

Resets the executable via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### make log-itm

```
make log-itm fcpu={HCLK in Hz}
```

Configures OpenOCD in tracing mode to output ITM channel 0 on SWO pin and
displays the serial output stream.
(\* *only ARM Cortex-M targets*)

```
 $ make log-itm fcpu=64000000
Open On-Chip Debugger 0.10.0
Licensed under GNU GPL v2
Info : The selected transport took over low-level target control.
loop: 57
loop: 58
loop: 59
loop: 60
loop: 61
```

See the `modm:platform:itm` module for details how to use the ITM as a logging
output.


#### make log-rtt

```
make log-rtt [channel={int}]
```

Configures OpenOCD in RTT mode to output the chosen channel (default 0) via a
simple telnet client. Disconnect with Ctrl+D.
(\* *only ARM Cortex-M targets*)

```
 $ make log-rtt
Info : rtt: Searching for control block 'modm.rtt.modm'
Info : rtt: Control block found at 0x20000008
loop: 57
loop: 58
loop: 59
loop: 60
loop: 61
```

See the `modm:platform:rtt` module for details how to use RTT for data transfer.


#### make library

```
make library profile={debug|release}
```

Generates only the static library `libmodm.a` without linking it to the
application.

```
 $ make library
Compiling C++·· {debug|release}/modm/ext/gcc/assert.o
    ...
Compiling C++·· {debug|release}/modm/src/modm/utils/dummy.o
Archiving······ {debug|release}/modm/libmodm.a
Indexing······· {debug|release}/modm/libmodm.a
```


#### make symbols

```
make symbols profile={debug|release}
```

Dumps the symbol table for your executable.

```
 $ make symbols
536871656 00000001 b (anonymous namespace)::nextOperation
536871657 00000001 b (anonymous namespace)::checkNextOperation
536871658 00000001 b (anonymous namespace)::error
536871444 00000001 b read_touch()::initialized
    ...
134228236 00000668 T I2C1_EV_IRQHandler
134224924 00001136 T otm8009a_init(unsigned char)
134221192 00001378 t _GLOBAL__sub_I_p
536871782 00002054 b (anonymous namespace)::txBuffer
```


#### make listing

```
make listing profile={debug|release}
```

Decompiles your executable into an annotated assembly listing.
This is very useful for checking and learning how the compiler translates C++
into assembly instructions:

```
 $ make listing
Listing········ {debug|release}/blink.lss
 $ less {debug|release}/blink.lss
    ...
Disassembly of section .text:
    ...
08000d74 <main>:
main():
./main.cpp:315

int
main()
{
 8000d74:   b508        push    {r3, lr}
    Board::initialize();
 8000d76:   f7ff fcc9   bl  800070c <_ZN5Board10initializeEv>
    Board::initializeDisplay();
 8000d7a:   f000 fd91   bl  80018a0 <_ZN5Board17initializeDisplayEv>
    Board::initializeTouchscreen();
 8000d7e:   f7ff fc55   bl  800062c <_ZN5Board21initializeTouchscreenEv>
    blink();
 8000d82:   f7ff feff   bl  8000b84 <_Z12blinkv>
    ...
```


#### make bin

```
make bin profile={debug|release}
```

Creates a binary file of your executable.

```
 $ make bin
Binary File···· {debug|release}/blink.bin
```


#### make hex

```
make hex profile={debug|release}
```

Creates a Intel-hex file of your executable.

```
 $ make bin
Hex File······· {debug|release}/blink.hex
```


## Information Tool

This tool generates a set of header files containing information about the
repository state.

Setting the `modm:build:info.git` option will generate a `<info_git.h>` header
file and add these two defines to the command line CPP options:

- `MODM_GIT_INFO`
- `MODM_GIT_STATUS`: defined only with option `Git+Status`.

!!! info "Increased build time"
    Since the git repository status can change at any time, it needs to be
    checked on every build. This adds less than a second to every build.

Setting the `modm:build:info.build` option will generate a `<info_build.h>`
header file and add this define to the command line CPP options:

- `MODM_BUILD_INFO`


!!! warning "Respect developers privacy"
    This information is placed into the firmware in **cleartext**, so it will
    be trivial to extract from a memory dump. Consider this information public
    as soon as it is uploaded to your target. Make sure you only use the
    information you absolutely need!


## Bitmap Tool

If the `modm:build:image.source` is defined as a path, it'll be searched
for `.pbm` files to convert into C++ data files using the `bitmap` tool.

See the `GraphicsDisplay::drawImage()` method in the `modm:ui:display` module
for how to use these generated files.
The directory is added to the include search paths, so the generated files
can be accessed as `#include <image.hpp>`.


[make]: https://www.gnu.org/software/make/manual/make.html
[bmp]: https://github.com/blacksphere/blackmagic
[gdbgui]: https://www.gdbgui.com
[phony]: https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
