# Build Systems

Before modm our HAL code generation was done as a SCons tool during 
compilation. This worked for us, however, made it very difficult to include
our code into other build systems.
We therefore made the code generation a separate step with lbuild.

There are two build script generators shipping with modm, which in cooperation
with the parent [`modm:build` module](module/modm-build) provide compilation,
uploading and debugging functionality.

### SCons

The `modm:build:scons` module extends the default SCons build system with many
custom utilities for a smooth integration of embedded tools.

[Read the module documentation for all the details](module/modm-build-scons).

### CMake

The `modm:build:cmake` module configures CMake
for compiling modm for STM32 targets.
This module ships with a Makefile that wraps all of the CMake commands.

[Read the module documentation for all the details](module/modm-build-cmake).