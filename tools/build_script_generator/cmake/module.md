# CMake Build Script Generator

[CMake][] is a widely used build tool supported by almost every IDE.

This module generates three files:

- a `modm/repo.cmake` file: configures all required CMake tools with
  the right settings (also using information from the `modm:build` module) to
  compile the modm library.
- a top-level `CMakeLists.txt`: configures CMake to build modm and your
  application.
- a top-level `Makefile`: provides a convenience wrapper for calling CMake from
  the command line.

This module is intended to be used for integration with IDEs that natively only
support CMake. It therefore only provides the bare minimum of features to compile
and upload your application to your target. For additional build tools features
see the `modm:build:scons` module.


## Make Methods

The `Makefile` wrapper provides these convenience methods.


#### make cmake

Generates the CMake build folders and initializes the build system.

```
 $ make cmake
    ...
-- Configuring done
-- Generating done
-- Build files have been written to: build/{project.name}/cmake-build-release
    ...
-- Build files have been written to: build/{project.name}/cmake-build-debug
```

!!! note "You must call this once before you can build!"
    If you forget to call this first, the build will fail with this error message:

    ```
     $ make build
    Error: build/cmake-build-release is not a directory
    make: *** [build-release] Error 1
    ```


#### make clean

Removes the CMake build artifacts.


#### make cleanall

Removes the entire build folder. You must then first call `make cmake` before
being able to build again.


#### make build

```
make build profile={debug|release}
```

Compiles your application into an executable using the release or debug profile.

```
 $ make build profile=release
Scanning dependencies of target blink_cmake
[  3%] Building C object CMakeFiles/blink_cmake.dir/modm/ext/cmsis/device/peripherals.c.o
[  7%] Building CXX object CMakeFiles/blink_cmake.dir/modm/src/modm/architecture/driver/atomic/flag.cpp.o
[ 19%] Building CXX object CMakeFiles/blink_cmake.dir/modm/src/modm/board/board.cpp.o
    ...
[ 96%] Building CXX object CMakeFiles/blink_cmake.dir/main.cpp.o
[100%] Linking CXX executable blink_cmake.elf
   text    data     bss     dec     hex filename
   1596      52   20468   22116    5664 blink_cmake.elf
[100%] Built target blink_cmake.bin
[100%] Built target blink_cmake.hex
[100%] Built target blink_cmake.lss
```


#### make size

```
make size profile={debug|release}
```

Displays the static Flash and RAM consumption of your target.

Example for the Blue Pill target:

```
 $ make size

Program:   1.4 KiB (2.2% used)
(.build_id + .fastcode + .fastdata + .hardware_init + .rodata +
 .table.copy.intern + .table.heap + .table.zero.intern + .text + .vector_rom)

Data:      3.0 KiB (15.1% used) = 20 B static (0.1%) + 3072 B stack (15.0%)
(.bss + .fastdata + .stack)

Heap:     17.0 KiB (84.9% available)
(.heap1)
```


#### make program

```
make program profile={debug|release} [port={serial-port}]
```

Writes the executable onto your target via AvrDude or OpenOCD.

```
 $ make program
[100%] Built target blink_cmake
[100%] Built target blink_cmake.bin
[100%] Built target blink_cmake.hex
[100%] Built target blink_cmake.lss
Open On-Chip Debugger 0.10.0
    ...
Info : using stlink api v2
Info : Target voltage: 3.171034
Info : stm32f1x.cpu: hardware has 6 breakpoints, 4 watchpoints
    ...
** Programming Started **
auto erase enabled
Info : device id = 0x20036410
Info : flash size = 64kbytes
    ...
wrote 2048 bytes from file build/cmake-build-release/blink_cmake.elf in 0.187893s (10.644 KiB/s)
** Programming Finished **
** Verify Started **
verified 1652 bytes in 0.104584s (15.426 KiB/s)
** Verified OK **
shutdown command invoked
```

#### make program-bmp

```
make program-bmp profile={debug|release} [port={serial-port}]
```

Writes the executable onto your target via Black Magic Probe.
(\* *only ARM Cortex-M targets*)


#### make debug

```
make debug profile={debug|release} ui={tui|web}
```

Launches OpenOCD in the background, then launches GDB in foreground with the
correct executable with text-based or [web-based gdbgui][gdbgui] UI. When GDB
exits, it stops the OpenOCD process.
(\* *only ARM Cortex-M targets*)

To use gdbgui you must have it installed via `pip install gdbgui`.

!!! tip "Choose the correct profile"
    When debugging, make sure to select the correct compilation profile. The
    firmware and the executable given to GDB have to be the some or you'll see
    GDB translate the program counter to the wrong code locations. When you
    suspect a bug in your firmware, consider that it was most likely compiled
    with the release profile, since that's the default. First try to
    `make debug profile=release`, and if that doesn't help, compile and
    `make program profile=debug` and try `make debug profile=debug` again.


#### make debug-coredump

```
make debug-coredump profile={debug|release} ui={tui|web}
```

Launches GDB for post-mortem debugging with the using the data in the
`coredump.txt` argument.
(\* *only ARM Cortex-M targets*)

See the `:platform:fault` module for details how to receive the coredump data.


#### make log-itm

```
make log-itm fcpu={HCLK in Hz}
```

Configures OpenOCD in tracing mode to output ITM channel 0 on SWO pin and
displays the serial output stream.

See the `modm:platform:itm` module for details how to use the ITM as a logging
output.


[cmake]: http://cmake.org
[gdbgui]: https://www.gdbgui.com
