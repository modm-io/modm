# CMake Build Script Generator

[CMake][] is a widely used build tool supported by almost every IDE.

This module generates three files:

- a `modm/cmake/ModmConfiguration.cmake` file: provides all required tools and
  compile settings to compile modm for your target.
- a `modm/CMakeLists.txt` file: provides the modm library target.

This module is intended to be used inside your IDE via its native CMake support
which will choose the compiler, build type, build path and generators for you.
Therefore you need to define your own top-level `CMakeLists.txt` and
`src/CMakeLists.txt` to use this module!

Please see [this example project for details](https://github.com/modm-ext/modm_starter_project).

!!! warning "Build Path is ignored"
    The `modm:build:build.path` option is ignored, since the IDE is responsible
    for setting it. Please consult your IDEs settings for that.

See the `modm:build:scons` or `modm:build:make` modules for build systems
designed for command line usage.


## CMake Targets

Several custom targets are provided to integrate embedded specific tasks into
your IDE.


#### size

Displays the static Flash and RAM consumption of your target.


#### program

Writes the executable onto your target via AvrDude or OpenOCD.


#### program-bmp

- option `MODM_BMP_PORT=auto` as string.

Writes the executable onto your target via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### debug

- option `MODM_DBG_UI=tui` in {`tui`, `gdbgui`}.

Launches OpenOCD in the background, then launches GDB in foreground with the
correct executable with text-based or [web-based gdbgui][gdbgui] UI. When GDB
exits, it stops the OpenOCD process.
(\* *only ARM Cortex-M targets*)

To use gdbgui you must have it installed via `pip install gdbgui`.

**We recommend using the IDE provided debug interface instead of this!**


#### debug-bmp

- option `MODM_BMP_PORT=auto` as string.

Launches GDB via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### debug-coredump

- option `MODM_DBG_UI=tui` in {`tui`, `gdbgui`}.

Launches GDB for post-mortem debugging with the using the data in the
`coredump.txt` argument.
(\* *only ARM Cortex-M targets*)

Use the `coredump` method to generate a coredump with a debugger attached,
otherwise see the `modm:platform:fault` module for details how to generate and
receive the coredump data from the device itself.


#### coredump

Launches GDB via OpenOCD and creates a `coredump.txt` file containing all
volatile memories and prints the GNU build ID of the firmware under debug.
Note that this command does not require an ELF file, so it can be used to
coredump any firmware whose ELF file is currently unavailable.
(\* *only ARM Cortex-M targets*)


#### coredump-bmp

Creates a coredump via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### reset

Resets the executable via OpenOCD.
(\* *only ARM Cortex-M targets*)


#### reset-bmp

Resets the executable via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### log-itm

- option `MODM_ITM_FCPU` in Hz

Configures OpenOCD in tracing mode to output ITM channel 0 on SWO pin and
displays the serial output stream.
(\* *only ARM Cortex-M targets*)

See the `modm:platform:itm` module for details how to use the ITM as a logging
output.


#### log-rtt

- option `MODM_RTT_CHANNEL=0` as integer.

Configures OpenOCD in RTT mode to output the chosen channel (default 0) via a
simple telnet client. Disconnect with Ctrl+D.
(\* *only ARM Cortex-M targets*)

See the `modm:platform:rtt` module for details how to use RTT for data transfer.


[cmake]: http://cmake.org
[gdbgui]: https://www.gdbgui.com
