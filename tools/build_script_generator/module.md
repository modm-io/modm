# Build System Generators

This parent module defines a common set of functionality that is independent of
the specific build system generator implementation.
This includes straight-forward options like project name and build path
but also more complicated configuration for programming your target via
AvrDude or OpenOCD and debugging via GDB.

**Note that this module does not compile your project, you will need to choose
the `modm:build:scons` or `modm:build:cmake` submodule for that, or provide
your own build system.**


## Compiler Options

We maintain a common set of compiler options for all build system generator, so
that they all have feature parity. We currently only support compiling modm
with GCC for AVR, ARM Cortex-M and x86/posix with the options mentioned in the
[offical GCC options documentation][options].

You can find all compiler options inside the generated build script for your
project, the options presented here are only the most important ones.

#### Shared for C and C++

- `-W{all, extra}`: a basic set of warnings.
- `-Werror={format, maybe-uninitialized, overflow, sign-compare}`: these warnings are treated as errors.
- `-f{data, function}-sections`: puts data and functions into their own linker section.
- `-funsigned-{char, bitfields}`: modm tries to use `stdint.h` types everywhere, but just in case.
- `-fwrapv`: integer overflows wrap around according to 2s complement.

For *release builds*:

- `-Os`: optimize for smaller size.

For *debug builds*:

- `-Og`: optimize for debugging experience.
- `MODM_DEBUG_BUILD`: this macro is only defined in debug profile. You can use
                      it with `#ifdef MODM_DEBUG_BUILD` to enable debug code.

#### Only C

- `-std=gnu2x`: use C23 with GNU extensions (for `asm volatile`).

#### Only C++

- `-std=c++23`: use C++23

For exception and RTTI flags, see `modm:stdc++` module.

#### Linker

- `--gc-section`: garbage collecting sections throws out a lot of unused data/code.
- `-L{linkdir} -Tlinkerscript.ld`: modm uses a custom linkerscript.

For target specific flags, see the `modm:platform:core` and related modules.

[options]: https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html


## Configuration

This module generates a common set of configuration files that are used by the
common tooling. Please note that these files are the *foundation* of more
extensive tooling available as Python scripts which are then again wrapped by
your chosen build system for convenience.


### OpenOCD

For accessing your ARM Cortex-M based device, we use OpenOCD by default and
generate a `modm/openocd.cfg` file with the target specific configuration:

- Search directories passed via the `path.openocd` collector.
- User configuration files passed via the `openocd.source` collector.
  Your custom `modm:build:openocd.cfg` is added here too.

You need to start openocd with this configuration file:

```sh
openocd -f modm/openocd.cfg
```

!!! warning "Be careful attaching to a running target"
    The OpenOCD implementation halts the target at least while the device's
    debug peripheral is initialized. Only connect to systems that cannot create
    any damage while being halted! For example halting motor controllers may
    damage motors!!


### AvrDude

Unfortunately AvrDude does not support a *project-specific* configuration file
like OpenOCD does (only a undocumented user config in `~/.avrduderc`), so there
is no convenient one-line command to issue. You have to use the wrapper support
of the specific build system or simply call AvrDude yourself via its command
line.


### GDB

A few commands are provided for convenience via the `modm/gdbinit` configuration:

- `reset`: resets the device and halts.
- `rerun`: resets the device and continues execution.
- `modm_coredump`: Dumps all volatile memories into a `coredump.txt` file.
                   See the `modm:platform:fault` module for details.
- `modm_build_id`: Finds and prints the GNU build id of the firmware.

GDB continues running the target after attaching, but *does not* load an ELF
file! Please pass the ELF file as a command line argument.

You can start your GDB session like so:

```sh
arm-none-eabi-gdb -x modm/gdbinit path/to/project.elf
```


## Generic Python Tools

We have written a number of pure Python tools to provide common functionality
that get wrapped by the build system.

Here is a selection of tools that have a command line interface, so you can call
them even without build system support in case you have a special setup.
Note that there are even more tools that can be called in Python only, so have
a look in your generated `modm/modm_tools` folder.

!!! tip "Add `modm_tools` to your Python path"
    To call `modm_tools` via module syntax, you need to add the generated modm
    folder to your Python path: `export PYTHONPATH=path/to/generated/modm`.
    You can also use the module in this repository directly.
