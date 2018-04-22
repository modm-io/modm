# CMake Build Script Generator

[CMake][] is a widely used build tool supported by almost every IDE.

This module generates two files:

- a top-level `CMakeLists.txt`: configures CMake to build modm and your
  application.
- a top-level `Makefile`: provides a convenience wrapper for calling CMake from
  the command line.

This module is intended to be used for integration with IDEs that natively only
support CMake. It therefore only provides the bare minimum of features and
can only be used to compile for ARM Cortex-M targets.

*The `modm:build:scons` module is the preferred build system for modm.*


## Make Methods

The `Makefile` wrapper provides these convenience methods.


#### make cmake

Generates the CMake build folders and initializes the build system.

```
 $ make cmake
    ...
-- Project : blink_cmake
-- Platform: stm32
-- Compiler: gcc
-- Part    : stm32f103c8t
-- Family  : f1
-- Core    : cortex-m3
-- Path    : build/{project.name}
-- Configuring done
-- Generating done
-- Build files have been written to: build/{project.name}/cmake-build-release
    ...
-- Build files have been written to: build/{project.name}/cmake-build-debug
```

!!! note "You must call this once before you can build!"
    If you forget to call this first, the build will fail with this error message:

    ```
     $ make build-release
    Error: build/cmake-build-release is not a directory
    make: *** [build-release] Error 1
    ```


#### make build-release
#### make build-debug

Compiles your application into an executable using the release or debug profile.

```
 $ make build-release
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


#### make upload-release
#### make upload-debug

Writes the executable onto your target via OpenOCD.
This is a convenience wrapper around the programming options and methods
defined in the `modm:build` module.

```
 $ make upload-release
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


#### make gdb
#### make gdb-release

Launches GDB with the debug or release executable.
This is just a convenience wrapper for the debug functionality defined in the
`modm:build` module.

**OpenOCD must already be running in the background**. Launch it by manually
calling it in another terminal:

```
openocd -f modm/openocd.cfg
```


#### make clean

Removes the CMake build artifacts.


#### make cleanall

Removes the entire build folders. You must then first call `make cmake` before
being able to build again


[cmake]: http://cmake.org