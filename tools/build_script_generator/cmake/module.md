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

Writes the executable onto your target via AvrDude or OpenOCD.
This is a convenience wrapper around the programming options and methods
defined in the `modm:build` module.
(\* *only AVR and ARM Cortex-M targets*)

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


#### make gdbtui
#### make gdbtui-release
#### make gdbgui
#### make gdbgui-release

Launches GDB with the text-based or with the [web-based GDBGUI](gdbgui) UI.
This is just a convenience wrapper for the debug functionality defined in the
`modm:build` module.
(\* *only ARM Cortex-M targets*)

**OpenOCD must already be running in the background**. Launch it by manually
calling `make openocd` in another terminal.

To use GDBGUI you must have it installed via `pip install gdbgui`.


#### make openocd

Starts OpenOCD with the modm specific configuration.


#### make clean

Removes the CMake build artifacts.


#### make cleanall

Removes the entire build folder. You must then first call `make cmake` before
being able to build again.


[cmake]: http://cmake.org
[gdbgui]: https://www.gdbgui.com
