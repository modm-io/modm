# Changelog

The purpose of this changelog is to give a good overview of the changes in xpcc.
It is an editorialized version of the git history, with features, PRs and fixes
chosen for their importance to xpcc users as judged by the maintainer.
This means that not every small commit makes it in here.

<details>
<summary>Changelog format</summary>

The release summaries are formatted as:

> #### yyyy-mm-dd: yyyyqn release
>
> Breaking changes: [list]  
> Major features: [list]  
> Fixes: [list]  
> Known bugs: [list]  
> New development board targets: [list]  
> New device drivers: [list]  
> Contributors: [list]
>
> PR #[GitHub] -> [release tag].

With a more detailed changelog below with this format:

> ##### yyyy-mm-dd: [summary]
>
> [detailed summary]
>
> PR #[GitHub] -> [sha1] (with [level] impact (on [scope])).  
> Tested in hardware by [contributor].

Note that `-> [sha1]` means "all changes up to and including this commit" which
needs to be interpreted in the context of the git history.
This usually corresponds to the changes in the Pull Request on GitHub though.

This changelog also labels the changes with an impact and usually also scopes
them to the relevant subset of xpcc code or targets.
This describes the anticipated impact of these changes on users of xpcc and has
four levels:

1. No impact mentioned: No influence. Adds new features and does not touch existing code.
2. Low impact: No expected influence. Modifies xpcc in a backwards compatible way.
3. Medium impact: Likely influence. Modifies xpcc in a non-backwards compatible way.
4. High impact: Definite influence. Breaks the API within the scope.

High impact changes are explicitly called out in the release notes for you to
pay attention to. Medium impact changes are also worth looking at.

</details>

## 2017-04-05: 2017q1 release

As this is our first official release it covers the last 12 months of
development since 2016-04-01.

Breaking changes:

- Fix implicit conversion of Flags
- Refactor `buffer` to `display_buffer` in `BufferedGraphicDisplay`
- Fix spelling of `SpiMaster::acquire`
- Namespace architecture/util.hpp macros
- `stm32f7_discovery` renamed to `stm32f746g_discovery`

Major features:

- Runtime error model using assertion handlers
- Memory traits to heap regions and allocators
- TLSF allocator
- Link against newlib-nosys and newlib-nano
- Unified interrupt declaration conventions
- Black Magic Probe programming support
- ARM Cortex-M7 with double precision FPU
- ZeroMQ as transport layer for XPCC
- Update STM32 header and device files
- Namespace architecture/util.hpp macros
- Coding conventions
- Porting guide

Major fixes:

- Fix implicit conversion of Flags
- Fix I2C master reception of 2B transfers
- Fix spelling of `SpiMaster::acquire`

Known bugs:

GPIO `connect` on STM32F1 is broken. They can be remapped only in groups,
however, the API allows invalid remapping. This cannot be fixed without
introducing a new API for that. See [#178][] for discussion.
A solution has been tested for modm, but isn't ready for xpcc.

New development board targets:

- NUCLEO-F031K6 as `nucleo_f031k6`
- NUCLEO-F303K8 as `nucleo_f303k8`
- NUCLEO-F401RE as `nucleo_f401re`
- NUCLEO-F411RE as `nucleo_f411re`
- NUCLEO-F429ZI as `nucleo_f429zi`
- STM32F769I-DISCO as `stm32f769_discovery`
- STM32F469I-DISCO as `stm32f469_discovery`
- STM32F103 board "Blue Pill" `blue_pill`

New device drivers:

- VL53L0X
- BME280
- ADNS9800
- FT6x06

Many thanks to all our contributors.
A special shoutout to first timers (🎉🎊):

- Antal Szabó ([@Sh4rK][])
- Christopher Durand ([@chris-durand][]) 🎉🎊
- Daniel Krebs ([@daniel-k][])
- Fabian Greif ([@dergraaf][])
- Georgi Grinshpun ([@georgi-g][])
- Julia Gutheil 🎉🎊
- Kevin Läufer ([@ekiwi][])
- Michael Thies ([@mhthies][]) 🎉🎊
- Nick Sarten ([@genbattle][]) 🎉🎊
- Niklas Hauser ([@salkinium][])
- Raphael Lehmann ([@rleh][]) 🎉🎊
- Sascha Schade ([@strongly-typed][])
- Tarik TIRE ([@7Kronos][]) 🎉🎊
- Tomasz Chyrowicz ([@tomchy][]) 🎉🎊

PR [#237][] -> [2017q1][].

<details>
<summary>Detailed changelog</summary>

#### 2017-04-03: Improve BoundedDeque

Adds `get()`, `rget()`, `[]` operator and `appendOverwrite()`,
`prependOverwrite()` to `BoundedDeque` to access elements by id, e.g.
to do a binary search on ordered elements in the deque and to use the
BoundedDeque as a ring buffer.

PR [#229][] -> [8f9b154][].  

#### 2017-04-02: Link against newlib-nosys

Adds `--specs=nosys.specs` to linker flags.

PR [#235][] -> [0e7e57f][] with low impact on ARM Cortex-M targets.  
Tested in hardware by [@salkinium][].

#### 2017-03-02: Add VL53L0X driver and example

This is some beautiful driver code.

PR [#234][] -> [553dceb][].  
Tested in hardware by [@chris-durand][].

#### 2017-03-02: Add NUCLEO-F401RE board and example

PR [#231][] -> [f472f7f][].  
Tested in hardware by [@strongly-typed][].

#### 2017-03-25: Fix implicit conversion of Flags

Conversion of Flags to an associated `FlagsGroup` will now work
correctly even when multiple enum constants or flag variables are
combined with logical operators.

The bool conversion of flags classes is implemented in terms of
`explicit operator bool()` to fix issues with function overloading
on Flags parameters.

The explicit operator will only allow implicit casts where contextual
conversion takes place, which is more restrictive than the previous
implementation. It will occur in the following contexts:

- conditions of if, while, for, do-while statements
- logical operators (&&, ||)
- negation (operator !)
- static_assert

This may break user code that requires implicit bool conversions on
other occasions, for instance in return statements.

PR [#230][] -> [fcf27a1][] with **high impact** due to breaking API change.

#### 2017-03-24: Refactor `buffer` to `display_buffer` in `BufferedGraphicDisplay`

This prevents confusion over which `buffer` to use when also inheriting from an
`I2cTransaction` class.

PR [#194][] -> [e1efaf4][] with **high impact** on display drivers due to breaking API change.

#### 2017-03-19: Add coding conventions

They've temporarily been lost.

PR [#224][] -> [c7adb48][].

#### 2017-03-19: Add NUCLEO-F031K6 board and example

PR [#221][] -> [f780c2a][].  
Tested in hardware by [@genbattle][].

#### 2017-03-19: Update and add all device headers and files

This replaces the STM32 device headers in ext/st/ with a git subtree
of modm-io/cmsis-header-stm32 and updates the DFG to work for STM32L
devices and adds all available device files.

PR [#226][] -> [da784bd][] with **medium impact** on STM32 targets.  
Tested in hardware by [@salkinium][].

#### 2017-01-25: Fix ZeroMQ communication backend

This fixes several issues in the ZeroMQ backend.

PR [#194][] -> [a8a2322][] with low impact.

#### 2016-12-10: Require Identifier for XPCC containers

In the progress of adding raw Ethernet frames as a backend for XPCC
communication, each container requires a unique id.

When using CAN as a backend filtering is done by component id.
Normally, each CAN controller has enough filters to accommodate up to
ten components per container. E.g. STM32F4 has 14 CAN filters.

When moving to Ethernet, the Ethernet MAC controller only has two or
six filters which may be not enough. The filters are more difficult to
configure.

The fifth byte of the MAC address will be used for the container Id and
the sixth byte for the component Id.
MAC filtering then can be easily implemented by filtering for the first
five bytes of the MAC address.

There is no overhead for CAN communication as container Id is omitted.

PR [#204][] -> [5f5934a][] with low impact.

#### 2016-12-10: Windows MinGW compilation support

PR [#202][] -> [dd3639b][] with low impact.  
Tested in hardware by [@7Kronos][].

#### 2016-11-20: Add unified interrupt declaration conventions

Adds macros for a unified interrupt declaration across all platforms
and regardless of being declared in C or C++ source code.
This allows us to change the interrupt invocation mechanism in the
future without modifying the source code.

In total four macros are added:

- `XPCC_ISR(vector, attributes...)` for declaring a static ISR with
                                    attributes (e.g. `xpcc_fastcode`).
- `XPCC_ISR_NAME(vector)` for getting the platform specific ISR name.
- `XPCC_ISR_DECL(vector)` for forward declarations of ISRs.
- `XPCC_ISR_CALL(vector)` for calling ISRs manually from code.

`XPCC_ISR_NAME(vector)` is backwards compatible, mapping to
`{vector}_vect` on AVR and `{vector}_IRQHandler` on ARM Cortex-M.
Consequently, the vector name is now used without `_vect` or
`_IRQHandler` suffixes and all occurrences have been renamed.

PR [#185][] -> [13904f8][] with low impact.

#### 2016-11-16: Add runtime error model using assertion handlers

This adds a mechanism for asserting runtime conditions on all platforms.
The application can specify failure handlers that get called on assertion
failure and can specify abandonment behavior depending on the failure.
The abandonment handler can also be overwritten to provide application
specific behavior, like blinking an LED or logging the failure.

For details see: http://blog.salkinium.com/xpccs-error-model

PR [#185][] -> [13904f8][] with low impact.  
Tested in hardware by [@salkinium][].

#### 2016-10-15: Allow setting compiler optimization level

The optimization level can be set in `project.cfg` as
`build.optimization=level` and overwritten using the command line
argument `optimization=level`.

PR [#196][] -> [c12a69b][].

#### 2016-10-03: Add BME280 driver and example

PR [#195][] -> [5e547ab][].  
Tested in hardware by [@strongly-typed][].

#### 2016-10-03: Add ZeroMQ as transport layer for XPCC and example

This feature adds a ZeroMQ implementation of the XPCC backend as a
replacement for TIPC on hosted targets and examples to show this new
functionality.

PR [#176][] -> [a00d3cc][].  
Tested in hardware by [@strongly-typed][].

#### 2016-10-02: Add Black Magic Probe support and example

PR [#194][] -> [b21f502][].  
Tested in hardware by [@strongly-typed][].

#### 2016-09-22: Add naming schema to device files

Adds a 'naming-schema' and 'invalid-device' tag to the device files to
allow to enumerate all valid device names from the device files alone.
Removes the size_id attribute for AVRs because it is redundant with
the name and not used by the AVR DFG merger.
Uses local imports in the Python code and therefore removes the need
to update the python path in every file.

PR [#189][] -> [d949fee][] with low impact.

#### 2016-09-21: Deduplicate and clean up linker scripts

Deduplication by using Jinja2 macros to deduplicate the linker script
formatting followed by clean up of unused linker sections.

PR [#188][] -> [a379e61][] with low impact on ARM Cortex-M targets.  
Tested in hardware by [@salkinium][].

#### 2016-09-20: Link against newlib-nano

Adds `--specs=nano.specs` to linker flags.
This saves up to 2kB of static data and ~1.5kB of code.

PR [#187][] -> [cb0a11e][] with **medium impact** on ARM Cortex-M targets.  
Tested in hardware by [@salkinium][].

#### 2016-09-17: Update DFG and fix driver issues

This change updates the device file generator (DFG) to generate the
patches we added to the device files manually now automatically.
Any drivers that needed minor changes due to this were updated.

PR [#183][] -> [08784cd][] with low impact.

#### 2016-09-15: Add IOStream formatter for function pointers

PR [#186][] -> [bb3fa3a][].

#### 2016-09-10: Namespace architecture/util.hpp macros

The `xpcc/architecture/util.hpp` macros have been renamed and
properly documented.
A new header `xpcc/architecture/legacy_macros.hpp` contains backwards
compatible mappings for application code. All occurrences of the non-
namespaced macros in xpcc have been replaced. The backwards-compatible
mapping can be disabled by declaring the `XPCC_DISABLE_LEGACY_MACROS`
macro to the compiler.

- ALWAYS_INLINE        -> xpcc_always_inline
- ATTRIBUTE_UNUSED     -> xpcc_unused
- ATTRIBUTE_WEAK       -> xpcc_weak
- ATTRIBUTE_ALIGNED(n) -> xpcc_aligned(n)
- ATTRIBUTE_PACKED     -> xpcc_packed
- ATTRIBUTE_FASTCODE   -> xpcc_fastcode
- ATTRIBUTE_FASTDATA   -> xpcc_fastdata
- ATTRIBUTE_MAY_ALIAS  -> xpcc_may_alias
- CONCAT(a,b)          -> XPCC_CONCAT(a,b)
- STRINGIFY(a)         -> XPCC_STRINGIFY(a)
- XPCC__ARRAY_SIZE(x)  -> XPCC_ARRAY_SIZE(x)
- likely(x)            -> xpcc_likely(x)
- unlikely(x)          -> xpcc_unlikely(x)

Note that the `ENUM_CLASS_FLAG` was replaced with `xpcc::Flags32` and
is not completely backwards compatible (`Interrupt` vs `Interrupt_t`).

PR [#182][] -> [9940a65][] with **high impact** due to breaking API change.

#### 2016-09-06: Simplify and clean up cortex/core driver

Includes better handling of platform specific startup code, a better
location for linkerscripts and minor code cleanup.

PR [#180][] -> [0dbf73c][] with low impact on ARM Cortex-M targets.  
Tested in hardware by [@salkinium][].

#### 2016-09-03: Add a porting guide

PR [#179][] -> [0dbf73c][].

#### 2016-08-25: Add ADNS9800 driver and example

PR [#175][] -> [26471ab][].  
Tested in hardware by [@strongly-typed][].

#### 2016-08-25: Add STM32F769I Discovery board and example

PR [#173][] -> [3c7cd31][].  
Tested in hardware by [@strongly-typed][].

#### 2016-08-25: Support ARM Cortex-M7 with double precision FPU

Adds build system and startup script support for ARM Cortex-M7 with double
precision FPU as well as CI tests.

PR [#173][] -> [c605416][] with low impact on STM32 devices.  
Tested in hardware by [@strongly-typed][].

#### 2016-08-25: Update STM32 header and device files

Update to the latest CMSIS device headers for STM32 and adds a few new device
files.

PR [#168][] -> [84d5bd0][] with **medium impact** on STM32 devices.  
Tested in hardware by [@salkinium][].

#### 2016-08-14: Fix spelling of `SpiMaster::acquire`

Apparently `acquire` is spelled with a `c`. This fix breaks the API, sorry!

PR [#167][] -> [fd1b109][] with **high impact** on drivers due to breaking API change.

#### 2016-08-04: Rename STM32F7 Discovery board name

There are more than one STM32F7 Discovery board available, this renames
the `stm32f7_discovery` board target to `stm32f746g_discovery`

PR [#165][] -> [637e074][] with **high impact** on STM32F7 targets.

#### 2016-07-20: Add Blue Pill board and examples

Add board support for STM32F103C8T6 Minimum System Development, aka.
"Blue Pill", and examples.

PR [#154][] -> [7ab0132][].  
Tested in hardware by [@strongly-typed][].

#### 2016-07-09: Add FT6x06 driver and example

PR [#155][] -> [63ad1d3][].  
Tested in hardware by [@salkinium][].

#### 2016-07-09: Add display driver for STM32F469I-DISCO

Adds initialization code and examples for the DSI and display found on the board.

PR [#155][] -> [0118a13][].  
Tested in hardware by [@salkinium][].

#### 2016-06-23: Add memory traits to heap regions and allocators

A heap table in the linkerscript describes the memory regions with their traits.
The allocator implementation reads this table to initialize the heaps.
To allocate memory, the heap that fits the requested traits best is chosen.

Three heap algorithms can be chosen from using the xpcc parameters:

- newlib's dlmalloc (default): Chooses largest continuous heap from page table.
- xpcc's block allocator: Chooses largest heap from page table.
- TLSF: Uses all heaps in the table.

Overloading of the new operator in C++ is provided:
```cpp
Foo *foo = new (xpcc::MemoryFastData) Foo(args);
```

Finally, linkerscripts can be extended to include additional memory regions
that include static data and bss as well as heap sections.

PR [#147][] -> [e2f9b4a][] with low impact on ARM Cortex-M targets with TLSF allocator.  
Tested in hardware by [@salkinium][].

#### 2016-06-12: Add NUCLEO-F303K8 board and example

PR [#145][] -> [6c1a111][].  
Tested in hardware by [@salkinium][].

#### 2016-06-12: Add NUCLEO-F429ZI board and example

PR ? -> [2504682][].  
Tested in hardware by [@salkinium][].

#### 2016-06-12: Replace SystemClock with manual implementation

This replaces the very dynamic `xpcc::stm32::SystemClock` with a fake
SystemClock class that is hardcoded to a specific clock speed.
This makes porting to new boards easier.

PR [#144][] -> [170f53a][] with **medium impact** on STM32 board targets.  
Tested in hardware by [@salkinium][].

#### 2016-05-29: Add operator overloads to Config/Value

This introduces the FlagsOperators class which contains nothing else but
the bitwise operators overloads and comparison functions.
This class in then inherited from for the Flags, Configurations and
Value classes, enabling transparent use of these operator overloads
between the classes.

PR [#143][] -> [a6519c3][] with low impact.

#### 2016-05-19: Add NUCLEO-F411RE board and example

PR [#138][] -> [e346020][].  
Tested in hardware by [@salkinium][].

#### 2016-05-17: Revert default allocator to newlib

Overwriting the weak functions in newlib regarding malloc does not work
correctly.

PR [#137][] -> [967c0a9][].

#### 2016-05-15: Fix I2C master reception of 2B transfers

This bug was discovered on the F103 platform, where the first byte of 2B
receive transfers was not acknowledged by the master.
This resulted in the slave device never placing the data for the 2nd
byte on the bus, effectively "sending" 0xff as the second byte.

PR [#129][] -> [408c309][].  
Tested in hardware by [@strongly-typed][].

#### 2016-05-03: Enable running unittests on AVR and STM32 targets

These changes fix the embedded test runners for AVR and STM32 targets,
and fixes bugs in the unit test source code that come from using the
cross compilers (float vs. double tolerances, type promotions, etc).

PR [#155][] -> [41ab22a][].

#### 2016-04-16: Add TLSF allocator and enable by default

These changes add the TLSF implementation from http://tlsf.baisoku.org
and enable it by default, completely replacing the newlib allocator.

Up to three arenas are constructed:

- SRAM: with `.heap1`, `.heap2` if available and `.heap3` if available
- CCM or DTCM: with `.heap0` if available
- ITCM: with `.heap4` if available

These three functions are provided:

- `malloc`: Uses SRAM by default, falls back to CCM if SRAM is full
- `realloc`: determines source arena by pointer address
- `free`: determines source arena by pointer address

PR [#115][] -> [ffa4e1b][] with **medium impact** on ARM Cortex-M targets.  
Tested in hardware by [@salkinium][].

</details>

## Changes previous to 2016-04

Please interpret the git history for older changes.


<!--
GitHub does not automatically link any references to PRs, users or commits so
we have to do it manually. Hooray for technology.
-->

<!-- Releases -->
[2017q1]: https://github.com/roboterclubaachen/xpcc/releases/tag/2017q1

<!-- Contributors -->
[@7Kronos]: https://github.com/7Kronos
[@chris-durand]: https://github.com/chris-durand
[@daniel-k]: https://github.com/daniel-k
[@dergraaf]: https://github.com/dergraaf
[@ekiwi]: https://github.com/ekiwi
[@genbattle]: https://github.com/genbattle
[@georgi-g]: https://github.com/georgi-g
[@mhthies]: https://github.com/mhthies
[@rleh]: https://github.com/rleh
[@salkinium]: https://github.com/salkinium
[@Sh4rK]: https://github.com/Sh4rK
[@strongly-typed]: https://github.com/strongly-typed
[@tomchy]: https://github.com/tomchy

<!-- Pull requests or Issues -->
[#115]: https://github.com/roboterclubaachen/xpcc/pull/115
[#129]: https://github.com/roboterclubaachen/xpcc/pull/129
[#137]: https://github.com/roboterclubaachen/xpcc/pull/137
[#138]: https://github.com/roboterclubaachen/xpcc/pull/138
[#143]: https://github.com/roboterclubaachen/xpcc/pull/143
[#144]: https://github.com/roboterclubaachen/xpcc/pull/144
[#145]: https://github.com/roboterclubaachen/xpcc/pull/145
[#147]: https://github.com/roboterclubaachen/xpcc/pull/147
[#154]: https://github.com/roboterclubaachen/xpcc/pull/154
[#155]: https://github.com/roboterclubaachen/xpcc/pull/155
[#155]: https://github.com/roboterclubaachen/xpcc/pull/155
[#155]: https://github.com/roboterclubaachen/xpcc/pull/155
[#165]: https://github.com/roboterclubaachen/xpcc/pull/165
[#167]: https://github.com/roboterclubaachen/xpcc/pull/167
[#168]: https://github.com/roboterclubaachen/xpcc/pull/168
[#173]: https://github.com/roboterclubaachen/xpcc/pull/173
[#173]: https://github.com/roboterclubaachen/xpcc/pull/173
[#175]: https://github.com/roboterclubaachen/xpcc/pull/175
[#176]: https://github.com/roboterclubaachen/xpcc/pull/176
[#178]: https://github.com/roboterclubaachen/xpcc/pull/178
[#179]: https://github.com/roboterclubaachen/xpcc/pull/179
[#180]: https://github.com/roboterclubaachen/xpcc/pull/180
[#182]: https://github.com/roboterclubaachen/xpcc/pull/182
[#183]: https://github.com/roboterclubaachen/xpcc/pull/183
[#185]: https://github.com/roboterclubaachen/xpcc/pull/185
[#185]: https://github.com/roboterclubaachen/xpcc/pull/185
[#186]: https://github.com/roboterclubaachen/xpcc/pull/186
[#187]: https://github.com/roboterclubaachen/xpcc/pull/187
[#188]: https://github.com/roboterclubaachen/xpcc/pull/188
[#189]: https://github.com/roboterclubaachen/xpcc/pull/189
[#194]: https://github.com/roboterclubaachen/xpcc/pull/194
[#194]: https://github.com/roboterclubaachen/xpcc/pull/194
[#194]: https://github.com/roboterclubaachen/xpcc/pull/194
[#195]: https://github.com/roboterclubaachen/xpcc/pull/195
[#196]: https://github.com/roboterclubaachen/xpcc/pull/196
[#202]: https://github.com/roboterclubaachen/xpcc/pull/202
[#204]: https://github.com/roboterclubaachen/xpcc/pull/204
[#221]: https://github.com/roboterclubaachen/xpcc/pull/221
[#224]: https://github.com/roboterclubaachen/xpcc/pull/224
[#226]: https://github.com/roboterclubaachen/xpcc/pull/226
[#229]: https://github.com/roboterclubaachen/xpcc/pull/229
[#230]: https://github.com/roboterclubaachen/xpcc/pull/230
[#231]: https://github.com/roboterclubaachen/xpcc/pull/231
[#234]: https://github.com/roboterclubaachen/xpcc/pull/234
[#235]: https://github.com/roboterclubaachen/xpcc/pull/235
[#237]: https://github.com/roboterclubaachen/xpcc/pull/237

<!-- Commits -->
[0118a13]: https://github.com/roboterclubaachen/xpcc/commit/0118a13
[08784cd]: https://github.com/roboterclubaachen/xpcc/commit/08784cd
[0dbf73c]: https://github.com/roboterclubaachen/xpcc/commit/0dbf73c
[0dbf73c]: https://github.com/roboterclubaachen/xpcc/commit/0dbf73c
[0e7e57f]: https://github.com/roboterclubaachen/xpcc/commit/0e7e57f
[13904f8]: https://github.com/roboterclubaachen/xpcc/commit/13904f8
[13904f8]: https://github.com/roboterclubaachen/xpcc/commit/13904f8
[170f53a]: https://github.com/roboterclubaachen/xpcc/commit/170f53a
[2504682]: https://github.com/roboterclubaachen/xpcc/commit/2504682
[26471ab]: https://github.com/roboterclubaachen/xpcc/commit/26471ab
[3c7cd31]: https://github.com/roboterclubaachen/xpcc/commit/3c7cd31
[408c309]: https://github.com/roboterclubaachen/xpcc/commit/408c309
[41ab22a]: https://github.com/roboterclubaachen/xpcc/commit/41ab22a
[553dceb]: https://github.com/roboterclubaachen/xpcc/commit/553dceb
[5e547ab]: https://github.com/roboterclubaachen/xpcc/commit/5e547ab
[5f5934a]: https://github.com/roboterclubaachen/xpcc/commit/5f5934a
[637e074]: https://github.com/roboterclubaachen/xpcc/commit/637e074
[63ad1d3]: https://github.com/roboterclubaachen/xpcc/commit/63ad1d3
[6c1a111]: https://github.com/roboterclubaachen/xpcc/commit/6c1a111
[7ab0132]: https://github.com/roboterclubaachen/xpcc/commit/7ab0132
[84d5bd0]: https://github.com/roboterclubaachen/xpcc/commit/84d5bd0
[8f9b154]: https://github.com/roboterclubaachen/xpcc/commit/8f9b154
[967c0a9]: https://github.com/roboterclubaachen/xpcc/commit/967c0a9
[9940a65]: https://github.com/roboterclubaachen/xpcc/commit/9940a65
[a00d3cc]: https://github.com/roboterclubaachen/xpcc/commit/a00d3cc
[a379e61]: https://github.com/roboterclubaachen/xpcc/commit/a379e61
[a6519c3]: https://github.com/roboterclubaachen/xpcc/commit/a6519c3
[a8a2322]: https://github.com/roboterclubaachen/xpcc/commit/a8a2322
[b21f502]: https://github.com/roboterclubaachen/xpcc/commit/b21f502
[bb3fa3a]: https://github.com/roboterclubaachen/xpcc/commit/bb3fa3a
[c12a69b]: https://github.com/roboterclubaachen/xpcc/commit/c12a69b
[c605416]: https://github.com/roboterclubaachen/xpcc/commit/c605416
[c7adb48]: https://github.com/roboterclubaachen/xpcc/commit/c7adb48
[cb0a11e]: https://github.com/roboterclubaachen/xpcc/commit/cb0a11e
[d949fee]: https://github.com/roboterclubaachen/xpcc/commit/d949fee
[da784bd]: https://github.com/roboterclubaachen/xpcc/commit/da784bd
[dd3639b]: https://github.com/roboterclubaachen/xpcc/commit/dd3639b
[e1efaf4]: https://github.com/roboterclubaachen/xpcc/commit/e1efaf4
[e2f9b4a]: https://github.com/roboterclubaachen/xpcc/commit/e2f9b4a
[e346020]: https://github.com/roboterclubaachen/xpcc/commit/e346020
[f472f7f]: https://github.com/roboterclubaachen/xpcc/commit/f472f7f
[f780c2a]: https://github.com/roboterclubaachen/xpcc/commit/f780c2a
[fcf27a1]: https://github.com/roboterclubaachen/xpcc/commit/fcf27a1
[fd1b109]: https://github.com/roboterclubaachen/xpcc/commit/fd1b109
[ffa4e1b]: https://github.com/roboterclubaachen/xpcc/commit/ffa4e1b
