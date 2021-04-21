# SCons Build Script Generator

[SCons][] is a software construction build system written in Python.
For a better embedded experience, we've extended it with modm-specific build
tools.

This module generates two files:

- a `modm/SConscript` file: configures all required SCons tools with
  the right settings (also using information from the `modm:build` module) to
  compile the modm library.
- a top-level BSD-licensed `SConstruct` file: configures additional, optional
  tools and sets up all the relevant SCons functions for your target.

The `SConscript` file is self contained and does not depend on anything outside of
the `modm/` directory. This allows it to be combined with `SConscript` of other
projects without clashing.

In fact, if you look at your generated `SConstruct` file, you'll notice that
it doesn't contain a lot of logic or specific data, it is only meant for calling
the right SCons tool with the right arguments.

We do not intend to serve every possible use-case with this module. **If you
need something special, write your own SConstruct file, maybe starting by
modifying ours.** It is intentionally BSD-licensed so that you do not have to
publish your changes to it.

Remember to set `modm:build:scons:include_sconstruct` to `False`, so that your
custom `SConstruct` does not get overwritten by `lbuild build`.
See the instructions inside our generated default `SConstruct` file.


## SCons Methods

This module generates these SCons methods depending on the target.


#### scons

Defaults to **scons build size**.

You can add these arguments to any of the SCons commands:

- `verbose=1`: gives a more verbose output, so you can, for example, check what
  options the compiler is called with.
- `profile=release`: Compile project with the release profile options (default).
- `profile=debug`: Compile project with the debug profile options.

For a description of the `release` and `debug` profiles, see the `modm:build`
module documentation.

!!! tip "Debug Profile"
    When working with the debug profile, make sure to add `profile=debug` to all
    commands, especially `scons program profile=debug` and
    `scons debug profile=debug`!

Some SCons commands take a `firmware={GNU Build ID or path/to/firmware.elf}`
argument that specifies which firmware to use for the command. It is useful in
combination with the `scons artifact` command to preserve a specific firmware
version for later.


#### scons build

```
scons build profile={debug|release}
```

Compiles your application into an executable.

Example for a STM32 target:

```
 $ scons build
Compiling C++·· {debug|release}/main.o
Compiling C···· {debug|release}/modm/ext/gcc/cabi.o
    ...
Compiling C++·· {debug|release}/modm/src/modm/utils/dummy.o
Archiving······ {debug|release}/modm/libmodm.a
Indexing······· {debug|release}/modm/libmodm.a
Linking········ /build/{debug|release}/blink.elf
```


#### scons -c

```
scons -c profile={debug|release}
```

Cleans the build artifacts.

```
 $ scons -c
Removed {debug|release}/main.o
Removed {debug|release}/modm/ext/tlsf/tlsf.o
    ...
Removed {debug|release}/modm/src/modm/ui/display/virtual_graphic_display.o
Removed {debug|release}/modm/src/modm/utils/dummy.o
Removed {debug|release}/modm/libmodm.a
Removed {debug|release}/blink.elf
Removed {debug|release}/blink.lss
```


#### scons size

```
scons size profile={debug|release} [firmware={hash or file}]
```

Displays the static Flash and RAM consumption of your target.

Example for a STM32 target with 16MB external heap:

```
 $ scons size
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


#### scons program

```
scons program profile={debug|release} [port={serial-port}] [firmware={hash or file}]
```

Writes the executable onto your target via Avrdude or OpenOCD.
This is a convenience wrapper around the programming options and methods
defined in the `modm:build` module.
(\* *only AVR and ARM Cortex-M targets*)

Example for a STM32 target:

```
 $ scons program
╭────────────── /build/{debug|release}/blink.elf
╰───OpenOCD───> stm32f469nih
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


#### scons program-fuses

```
scons program-fuses profile={debug|release} [firmware={hash or file}]
```

Writes all fuses onto your target connected to avrdude. See the
`modm:platform:core` module for how to define the fuse values.
(\* *only AVR targets*)


#### scons program-dfu

```
scons program-dfu profile={debug|release} [firmware={hash or file}] [delay={seconds}]
```

Writes the executable onto your target via Device Firmware Update (DFU) over USB.
A DFU bootloader is available on many STM32 microcontrollers
and can be accessed by pressing the BOOT0-Button during startup.

Some DFU devices require additional delay to re-enumerate, which you can specify
with the `delay` parameter (default is 5 seconds).
(\* *only ARM Cortex-M targets*)

```
$ scons program-dfu
Binary File···· /build/{debug|release}/blink.bin
╭────────────── /build/{debug|release}/blink.bin
╰─────DFU─────> stm32f469nih
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
scons: done building targets.
```


#### scons program-bmp

```
scons program-bmp profile={debug|release} [port={serial-port}] [firmware={hash or file}]
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
$ scons program-bmp port=/dev/tty.usbmodemDEADBEEF
╭─Black─Magic── /build/{debug|release}/blink.elf
╰────Probe────> stm32f103rbt6
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
scons: done building targets.
```


#### scons program-remote

```
scons program-remote profile={debug|release} [host={ip or hostname}] [firmware={hash or file}]
```

Writes the executable onto your target connected to a remote OpenOCD process
running on your own computer (host=`localhost`) or somewhere else.


#### scons run

Compiles and executes your program on your computer.
(\* *only Hosted targets*)


#### scons debug

```
scons debug profile={debug|release} ui={tui|web} [firmware={hash or file}]
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
    with the release profile, since that's the default. First try to `scons
    debug profile=release`, and if that doesn't help, compile and `scons
    program profile=debug` and try `scons debug profile=debug` again.


#### scons debug-bmp

```
scons debug-bmp profile={debug|release} ui={tui|web} port={serial-port} [firmware={hash or file}]
```

Launches GDB to debug via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### scons debug-coredump

```
scons debug-coredump profile={debug|release} ui={tui|web} \
                     coredump={path/to/coredump.txt} \
                     [firmware={GNU Build ID or path/to/firmware.elf}]
```

Launches GDB for post-mortem debugging with the firmware identified by the
(optional) `firmware={hash or filepath}` argument using the data from the
`coredump={filepath}` argument.
(\* *only ARM Cortex-M targets*)

See the `modm:platform:fault` module for details how to receive the coredump data.


#### scons program-remote

```
scons debug-remote profile={debug|release} ui={tui|web} [host={ip or hostname}] [firmware={hash or file}]
```

Debugs the executable via a remote OpenOCD process running on your own computer
(localhost is default) or somewhere else.
(\* *only ARM Cortex-M targets*)


#### scons reset

```
scons reset
```

Resets the executable via OpenOCD.
(\* *only ARM Cortex-M targets*)


#### scons reset-bmp

```
scons reset-bmp [port={serial}]
```

Resets the executable via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### scons reset-remote

```
scons reset-remote [host={ip or hostname}]
```

Resets the executable via a remote OpenOCD process running on your own computer
(localhost is default) or somewhere else.
(\* *only ARM Cortex-M targets*)


#### scons log-itm

```
scons log-itm fcpu={HCLK in Hz}
```

Configures OpenOCD in tracing mode to output ITM channel 0 on SWO pin and
displays the serial output stream.
(\* *only ARM Cortex-M targets*)

```
 $ scons log-itm fcpu=64000000
╭───OpenOCD───> Single Wire Viewer
╰─────SWO────── stm32f103rbt6
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


#### scons log-rtt

```
scons log-rtt [channel={int}]
```

Configures OpenOCD in RTT mode to output the chosen channel (default 0) via a
simple telnet client. Disconnect with Ctrl+D.
(\* *only ARM Cortex-M targets*)

```
 $ scons log-rtt
╭───OpenOCD───> Real Time Transfer
╰─────RTT────── stm32f103rbt6
Info : rtt: Searching for control block 'modm.rtt.modm'
Info : rtt: Control block found at 0x20000008
loop: 57
loop: 58
loop: 59
loop: 60
loop: 61
```

See the `modm:platform:rtt` module for details how to use RTT for data transfer.


#### scons library

```
scons library profile={debug|release}
```

Generates only the static library `libmodm.a` without linking it to the
application.

```
 $ scons library
Compiling C++·· {debug|release}/modm/ext/gcc/assert.o
    ...
Compiling C++·· {debug|release}/modm/src/modm/utils/dummy.o
Archiving······ {debug|release}/modm/libmodm.a
Indexing······· {debug|release}/modm/libmodm.a
```


#### scons symbols

```
scons symbols profile={debug|release} [firmware={hash or file}]
```

Dumps the symbol table for your executable.

```
 $ scons symbols [firmware={hash or file}]
Show symbols for '{debug|release}/blink.elf':
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


#### scons listing

```
scons listing profile={debug|release} [firmware={hash or file}]
```

Decompiles your executable into an annotated assembly listing.
This is very useful for checking and learning how the compiler translates C++
into assembly instructions:

```
 $ scons listing
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


#### scons bin

```
scons bin profile={debug|release} [firmware={hash or file}]
```

Creates a binary file of your executable.

```
 $ scons bin
Binary File···· {debug|release}/blink.bin
```


#### scons hex

```
scons hex profile={debug|release} [firmware={hash or file}]
```

Creates a Intel-hex file of your executable.

```
 $ scons hex
Hex File······· {debug|release}/blink.hex
```


#### scons artifact

```
scons artifact profile={debug|release}
```

Caches the ELF and binary file of the newest compiled executable identified by
the hash of the binary file in `artifacts/{hash}.elf`. You can change this path
with the `modm:build:scons:path.artifact` option.

```
 $ scons artifact
╭───Artifact─── /build/release/blink.elf
╰────Cache────> artifacts/0214523ab713bc7bdfb37d902e65dae8305f4754.elf
```


#### scons qtcreator

Generates several files so that the project can be imported into Qt Creator via
the `.creator` file importer. Note, that no compiliation or debugging features
are supported, this is only meant for using the IDE as an editor.

!!! warning "Consider this an unstable feature"


## XPCC Generator Tool

The `modm:communication:xpcc:generator` module contains the Python tools to
translate the XPCC XML declarations into various language implementations.
This module contains a SCons wrapper tool, that understands the XML dependencies
and automatically updates the generated files when it becomes necessary.

The wrapper tool is automatically used when the generator module is detected,
and its options are evaluated for the wrapper as follows:

```py
env.XpccCommunication(
    xmlfile=options["::xpcc:generator:source"],
    container=options["::xpcc:generator:container"],
    path=options["::xpcc:generator:path"],
    namespace=options["::xpcc:generator:namespace"]
)
```

The generated files are available as a top-level `#include <identifiers.hpp>`.


## Information Tool

Our `info` SCons tool generates a set of header files containing information
about the repository state.

A call to `env.InfoGit(with_status={True, False})` will generate a `<info_git.h>`
header file and add these two defines to the command line CPP options:

- `MODM_GIT_INFO`
- `MODM_GIT_STATUS`: defined only if called with `with_state=True`.

You can enable this by setting the `modm:build:info.git` option.

!!! info "Increased build time"
    Since the git repository status can change at any time, it needs to be
    checked on every build. This adds less than a second to every build.

A call to `env.InfoBuild()` will generate a `<info_build.h>` header file and add
this define to the command line CPP options:

- `MODM_BUILD_INFO`

You can enable this by setting the `modm:build:info.build` option.

!!! warning "Respect developers privacy"
    This information is placed into the firmware in **cleartext**, so it will
    be trivial to extract from a memory dump. Consider this information public
    as soon as it is uploaded to your target. Make sure you only use the
    information you absolutely need!


## Bitmap Tool

If the `modm:build:image.source` is defined as a path, it'll be searched
for `.pbm` files to convert into C++ data files using the `bitmap` tool:

```py
source, header = env.Bitmap(bpm_file)
```

See the `GraphicsDisplay::drawImage()` method in the `modm:ui:display` module
for how to use these generated files.
The directory is added to the include search paths, so the generated files
can be accessed as `#include <image.hpp>`.


[scons]: http://scons.org
[bmp]: https://github.com/blacksphere/blackmagic
[gdbgui]: https://www.gdbgui.com
