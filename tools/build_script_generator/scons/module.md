# SCons Build Script Generator

[SCons][] is a software construction build system written in Python.
For a better embedded experience, we've extended it with a custom build tools
[by the DLR][scons_tools] as well as our own modm-specific build tools.

This module generates two files:

- a `modm/SConscript` file: configures all required SCons tools with
  the right settings (also using information from the `modm:build` module) to
  compile the modm library.
- a top-level `SConstruct` file: configures additional, optional tools and sets
  up all the relevant SCons functions for your target.

The `SConscript` file is self contained and does not depend on anything outside of
the `modm/` directory. This allows it to be combined with `SConscript` of other
projects without clashing.

In fact, if you look at your generated `SConstruct` file, you'll notice that
it doesn't contain a lot of logic or specific data, it is only meant for calling
the right SCons tool with the right arguments.

We do not intend to serve every possible use-case with this module.
**If you need something special, write your own SConstruct file, maybe
starting by modifying ours.**
Remember to set `modm:build:scons:include_sconstruct` to `False`, so that your
custom `SConstruct` does not get overwritten.
See the instructions inside our generated default `SConstruct`.


## SCons Methods

This module generates these SCons methods depending on the target.


#### scons

Defaults to **scons build**.

You can add these arguments to any of the scons commands:

- `verbose=1`: gives a more verbose output, so you can, for example, check what
  options the compiler is called with.
- `profile=release`: Compile project with the release profile options (default).
- `profile=debug`: Compile project with the debug profile options.

For a description of the `release` and `debug` profiles, see the `modm:build`
module documentation.

!!! tip "Debug Profile"
    When working with the debug profile, make sure to add `profile=debug` to all
    commands, especially `scons program profile=debug` and
    `scons gdb profile=debug`!


#### scons build

Compiles your application into an executable.

Example for a STM32 target:

```
 $ scons build
Compiling C++·· build/release/main.o
Compiling C···· build/release/modm/ext/tlsf/tlsf.o
    ...
Compiling C++·· build/release/modm/src/modm/ui/display/virtual_graphic_display.o
Compiling C++·· build/release/modm/src/modm/utils/dummy.o
Create Library· build/release/modm/libmodm.a
Indexing······· build/release/modm/libmodm.a
Linking········ build/release/game_of_life.elf
```


#### scons size

Displays the static Flash and RAM consumption of your target.
(\* *only STM32 targets*)

Example for a STM32 target with 16MB external heap:

```
 $ scons size
Memory usage:.. build/release/game_of_life.elf

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

Writes the executable onto your target via Avrdude or OpenOCD.
This is a convenience wrapper around the programming options and methods
defined in the `modm:build` module.
(\* *only AVR and STM32 targets*)

Example for a STM32 target:

```
 $ scons program
.----OpenOCD--- build/release/game_of_life.elf
'-------------> stm32f469nih
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
wrote 16384 bytes from file build/release/game_of_life.elf in 0.589736s (27.131 KiB/s)
** Programming Finished **
** Verify Started **
verified 13064 bytes in 0.296308s (43.056 KiB/s)
** Verified OK **
shutdown command invoked
```


#### scons run

Executes your project on your computer.
(\* *only Hosted targets*)


#### scons gdb

Launches OpenOCD in the background, then launches GDB in foreground with the
correct executable. When GDB exits, it stops the OpenOCD process.
(\* *only STM32 targets*)

This is just a convenience wrapper for the debug functionality defined in the
`modm:build` module.

!!! tip "Choose the correct profile"
    When debugging, make sure to select the correct compilation profile.
    The firmware and the executable given to GDB have to be the some or you'll
    see GDB translate the program counter to the wrong code locations.
    When you suspect a bug in your firmware, consider that it was most likely
    compiled with the release profile, since that's the default.
    First try to `scons gdb profile=release`, and if that doesn't help, compile
    and `scons program profile=debug` and try `scons gdb profile=debug` again.


#### scons symbols

Dumps the symbol table for your executable.

```
 $ scons symbols
Show symbols for 'build/release/game_of_life.elf':
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

Decompiles your executable into an annotated assembly listing.
This is very useful for checking and learning how the compiler translates C++
into assembly instructions:

```
 $ scons listing
Ext. Listing··· build/release/game_of_life.lss
 $ less build/release/game_of_life.lss
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
    game_of_life();
 8000d82:   f7ff feff   bl  8000b84 <_Z12game_of_lifev>
    ...
```


#### scons bin

Creates a binary file of your executable.

```
 $ scons bin
Binary File···· build/release/game_of_life.bin
```


#### scons artifact

Caches the ELF and binary file of the newest compiled executable identified by
the hash of the binary file in `{build_path}/artifacts/{hash}.elf`.

```
 $ scons artifact
Cache Artifact· build/release/game_of_life.elf
```


#### scons -c

Cleans the build artifacts.

```
 $ scons -c
Removed build/release/main.o
Removed build/release/modm/ext/tlsf/tlsf.o

Removed build/release/modm/src/modm/ui/display/virtual_graphic_display.o
Removed build/release/modm/src/modm/utils/dummy.o
Removed build/release/modm/libmodm.a
Removed build/release/game_of_life.elf
Removed build/release/game_of_life.lss
```


#### scons cmakewrapper

Generates a `CMakeLists.txt` file that wraps the scons commands so
that the project can be imported into IDE with CMake support, like CLion.

!!! warning "Consider this an unstable feature"


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

Our `info` SCons tool generates a set of header files containing preprocessor
defines with the repository state.

A call to `env.InfoGit(with_status={True, False})` will generate a `<info_git.h>`
header file and add these two defines to the command line CPP options:

- `MODM_GIT_INFO`
- `MODM_GIT_STATUS`: defined only if called with `with_state=True`.

You can enable this by setting the `modm:build:scons:info.git` option.

A call to `env.InfoBuild()` will generate a `<info_build.h>` header file and add
this define to the command line CPP options:

- `MODM_BUILD_INFO`

You can enable this by setting the `modm:build:scons:info.build` option.

!!! warning "Respect developers privacy"
    This information is placed into the firmware in **cleartext**, so it will
    be trivial to extract from a memory dump. Consider this information public
    as soon as it is uploaded to your target. Make sure you only use the
    information you absolutely need!


### Git Information

These are the values defined as `const char *` strings in `<info_git.h>`:

- `MODM_GIT_SHA`: commit hash: `%H`.
- `MODM_GIT_SHA_ABBR`: short commit hash: `%h`.
- `MODM_GIT_SUBJECT`: commit subject as text: `%s`.
- `MODM_GIT_AUTHOR`: author name: `%an`.
- `MODM_GIT_AUTHOR_EMAIL`: author email: `%ae`.
- `MODM_GIT_AUTHOR_DATE`: authoring date: `%ad`.
- `MODM_GIT_AUTHOR_DATE_TIMESTAMP`: authoring date as Unix timestamp: `%at`.
- `MODM_GIT_COMMITTER`: committer name: `%cn`.
- `MODM_GIT_COMMITTER_EMAIL`: committer email: `%ce`.
- `MODM_GIT_COMMITTER_DATE`: committer date: `%cd`.
- `MODM_GIT_COMMITTER_DATE_TIMESTAMP`: committer das as Unix timestamp: `%ct`.
- `MODM_GIT_CONFIG_USER_NAME`: local user name: `user.name`.
- `MODM_GIT_CONFIG_USER_EMAIL`: local user email: `user.email`.

If called `with_status=True`, this information is added as well:

- `MODM_GIT_MODIFIED`: number of modified files: `M`.
- `MODM_GIT_ADDED`: number of added files: `A`.
- `MODM_GIT_DELETED`: number of deleted files: `D`.
- `MODM_GIT_RENAMED`: number of renamed files: `R`.
- `MODM_GIT_COPIED`: number of copied files: `C`.
- `MODM_GIT_UPDATED_NOT_MERGED`: number of updated, but not merged files: `U`.
- `MODM_GIT_UNTRACKED`: number of untracked files: `?`.

This is the output of the `example/linux/git` project showing an unclean
repository state with uncommitted changes. This can give you a few hints as to
where a firmware came from and help you pinpoint the source of a bug or feature.

```
Info:    Local Git User:
Info:    Name:  Name Surname
Info:    Email: name.surname@provider.com
Info:    Last Commit:
Info:    SHA:             1b5a9a642857182161a615039c92907e59881614
Info:    Abbreviated SHA: 1b5a9a642
Info:    Subject:         wip
Info:
Info:    Author:
Info:    Name:      Name Surname
Info:    Email:     name.surname@provider.com
Info:    Date:      Tue Jul 17 22:23:20 2018 +0200
Info:    Timestamp: 1531859000
Info:
Info:    Committer:
Info:    Name:      Name Surname
Info:    Email:     name.surname@provider.com
Info:    Date:      Tue Jul 17 22:23:20 2018 +0200
Info:    Timestamp: 1531859000
Info:
Info:    File Status:
Info:    Modified:  10
Info:    Added:     0
Info:    Deleted:   0
Info:    Renamed:   0
Info:    Copied:    0
Info:    Untracked: 6
```


### Build Information

These are the values defined as `const char *` strings in `<info_build.h>`:

- `MODM_BUILD_PROJECT_NAME`: as defined in the `modm:build:project.name` option.
- `MODM_BUILD_MACHINE`: machine information.
- `MODM_BUILD_USER`: user information.
- `MODM_BUILD_OS`: OS version string (best effort, may not be useful!).
- `MODM_BUILD_COMPILER`: compiler information.

The output of the `examples/linux/build_info` running on macOS:

```
Info:    Project:  build_info
Info:    Machine:  name.local
Info:    User:     user
Info:    Os:       Mac 10.12.6 (x86_64)
Info:    Compiler: g++-8 (Homebrew GCC 8.1.0) 8.1.0
```


## Bitmap Tool

If the `modm:build:scons:image.source` is defined as a path, it'll be searched
for `.pbm` files to convert into C++ data files using the `bitmap` tool:

```py
source, header = env.Bitmap(bpm_file)
```

See the `GraphicsDisplay::drawImage()` method in the `modm:ui:display` module
for how to use these generated files.
The directory is added to the include search paths, so the generated files
can be accessed as `#include <image.hpp>`.


[scons]: http://scons.org
[scons_tools]: https://github.com/modm-io/scons-build-tools
