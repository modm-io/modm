# Porting xpcc projects to modm

modm is the direct successor to xpcc and fully replaces it. xpcc is declared deprecated on the 2018q2 release and the repository is archived on the 2018q4 release. We‘ve already manually forward ported all changes to modm and will continue to do so, however, we‘d appreciate it if you contributed to modm directly from now on.

This porting guide is kept up-to-date for the state of xpcc and modm until the 2018q4 release. modm will then take over the quarterly release schedule from xpcc. If you want to port to a newer modm release, we recommend first porting to the modm 2018q4 release as described in this guide, then refactoring your code using the modm Changelog to be compatible with the latest modm release.

modm‘s largest improvement over xpcc is its generic way of generating the HAL by using the explicit code generation capabilities of lbuild. In xpcc this was hacked into the SCons build system, and was very unintuitive to use and extend.
Instead, modm now is a *library generator*, introducing an explicit code generation step *before* compilation.  You provide lbuild with a configuration file containing the library options (what modules you want for what target) and lbuild will then create a specific modm library for your project. You can even choose what build system you would like to use (at the time of writing: SCons or CMake) or just use your own, since the generated library is build system agnostic.
Please read the section on [How modm works](https://modm.io/how-modm-works/) for more details on this process.

This is very different from xpcc, where the generated sources where implicitly generated somewhere in the build directory and you were forced to use SCons for building.
In modm we want you to be aware of the generated sources, so you are encouraged to commit the generated modm library and build system into your project VCS.
This will also make it easier to review what has changed when you upgrade modm.

We decided to change the name to modm (Modular Object-oriented Development for
Microcontrollers) to resolve the confusion when talking about the XPCC (Cross Platform Component Communication) protocol (the RPC implementation we use inside our robots) versus the xpcc framework, which was initially build around this protocol, but since then became much more generic.


## Changes

We‘ve created a Python3 script to automate as much of the refactoring as possible. 
It will rewrite the source code in the folders you provide as arguments.
If a `project.cfg` file is present, it will translate this to a `project.xml` file,
otherwise it'll output the detected modm modules on the command line.

```sh
python3 /path/to/modm/tools/scripts/xpcc_porting.py src_folder ../other/src/folders
```

However, this script may not have enough information in the sources to port
everything, so please read on to see what you still need to fix manually. 

modm continues to support AVR and STM32 targets as well as all three major desktop operating systems. However, support for LPC, Xmega and some custom STM32 boards has been removed.
**Please note that STM32 device identifiers in modm have an additional character denoting the package id!** You must provide this information, the porting script does not have enough information.

modm has been grouped into small modules which allows for a more fine-grained library generation with fewer files that are not needed. This meant adding one `*.lb` file for every module and sometimes a few `*.md` files for documentation.

We are now using a simpler collection of SCons tools maintained by the DLR.
We‘ve integrated it so that the main SCons commands are the same, however, some tools have not been ported or have a different feature set than before.

### API changes

Apart from the code generation changes in lbuild there are also a number of breaking code changes. modm is still very much continuing the C++ concepts laid out in xpcc, however, modm is not a drop-in replacement, so you will have to refactor your projects.

#### Namespaces

In parallel to changing the libraries name, we‘ve also changed the top-level namespace to `modm`, EXCEPT for the XPCC protocol, which remains in namespace `xpcc`.

The target specific namespaces have been renamed from `modm::{at90, attiny, atmega, xmega, stm32}` to simply `modm::platform`. This namespace contains all classes that are specific to the target and should not be used platform-independent code (use the interfaces classes instead).

The remaining sub-namespaces have not been altered.

#### Includes

All modm specific include paths now start with `modm` not `xpcc`.
`#include <xpcc/header.hpp>` becomes `#include <modm/header.hpp>`.

The `<modm/architecture.hpp>` header does NOT include `<modm/platform.hpp>`, so you may have to change that manually if the platform classes are not found.
When using modm-provided BSP header, you need to `#include <modm/board.hpp>` instead, which includes the platform headers too.

#### Preprocessor

All preprocessor definitions were refactored to be prefixed with either `MODM_` or `modm_`.
All double underscores between defines `WORD__WORD` were replaced by single underscores `WORD_WORD`.

#### GPIOs

The entire GPIO implementation has been rewritten from scratch for modm. We‘ve encountered several issues about the xpcc implementation that we could only solve with the much more accurate data of modm-devices and more generic code generation capabilities of lbuild.

The main issue was that the signal connector API `Gpio::connect(Signal);` was not able to map to group signal remaps as used in the Xmega and STM32F1 targets. Furthermore, the signal naming did not reflect the Datasheet naming, and its implemention was dependent on the peripheral driver code.

In modm the connect method is part of the peripheral driver and gets passed a number of pins-signal pairs in independent order: `Uart::connect<GpioD0::Tx, GpioD1::Rx>();`. This allows to check for group remap conflicts at compile time.
In addition, the peripheral driver can configure the pins as needed by the interface.
In this implementation, the peripheral connect method is just a wrapper for convenience, the GPIO implementation does not depend on any peripheral drivers, so it can be used on its own. See [these issues for some details]().

GPIO ports have been rewritten as well to be more efficient, however the code is backwards compatible with the exception of `DataOrder::Reverse` for `GpioPort`, which is now done with a negative index instead: `GpioPort<GpioC7, -8>`. However, this feature was very rarely if ever used. See PR #12 for details.

Classes like `GpioUnused` and `GpioInverted` have been moved into `namespace modm::platform`.

### External code

External code (in `ext/`) is now managed via git submodules, so you need to clone modm recursively! We‘ve taken care to minimize the size of these modules, for some very large repositories (CMSIS, CubeHAL), we‘ve extracted the relevant sources into our own, much smaller repositories.

### Folder structure

A number of folders and files were moved, created or deleted:

- `src/xpcc` -> `src/modm`.
- `src/modm/architecture/platform/drivers` -> `src/modm/platform`.
- `src/modm/architecture/platform/board` -> `src/modm/board`.
- `src/modm/architecture/platform/devices` replaced by the `ext/modm-devices` submodule.
- `src/tools/device_file*` have been moved to the modm-devices project.

All the distributed `test/` folders containing unit tests were moved to `test/modm` with the same relative folder structure.

### Testing

The testing system has been refactored to use the modularized modm. It is now simpler to use than before. Additionally, you can compile modm for all supported targets now. See the [modm guide on testing for details](https://modm.io/guide/testing).
