# Changelog

The purpose of this changelog is to give a good overview of the changes in modm.
It is an editorialized version of the git history, with features, PRs and fixes
chosen for their importance to modm users as judged by the maintainer.
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
them to the relevant subset of modm code or targets.
This describes the anticipated impact of these changes on users of modm and has
four levels:

1. No impact mentioned: No influence. Adds new features and does not touch existing code.
2. Low impact: No expected influence. Modifies modm in a backwards compatible way.
3. Medium impact: Likely influence. Modifies modm in a non-backwards compatible way.
4. High impact: Definite influence. Breaks the API within the scope.

High impact changes are explicitly called out in the release notes for you to
pay attention to. Medium impact changes are also worth looking at.

</details>

## 2018-01-07: 2017q4 release

This release covers everything from the 2017q3 release on 2017-10-01 and has been
tested with avr-gcc v5.4.0 from Atmel and arm-none-eabi-gcc 2017q4 from Arm. 

Breaking changes:

- No breaking changes.

Major features:

- SCons 3 and Python 3 support for built tools
- All targets build with C++14 by default
- Rosserial support
- LPC support (revival)

Major fixes:

- Fix the I2C driver on STM32L0/L4 for writes/reads > 255 bytes.
- Fix matrix multiplication and LU decomposition
- Fix `GpioPort` setOutput()/setInput()/configure() on STM32
- Fix Hd44780 initialization for 4-bit bus
- Fix GCC7-specific compile errors and warnings

Known bugs:

- modm may generate separate IRQ handlers for shared interrupts. See [#88][].
- GPIO `connect` on STM32F1 is still broken. See [#178][] for discussion.
  The API from MODM will not be backported to modm however.
- STM32F107 does not compile due to the HAL trying to remap USB. See [#268][].
- SCons build system emits multiple non-critical warnings. See [#286][].
- avr-gcc 7 complains about dynamic initialization put into program memory area. See [#314][].

New development board targets:

- No new development board targets

New device drivers:

- No new device drivers

Many thanks to all our contributors.
A special shoutout to first timers (🎉🎊):

- Álan Crístoffer ([@acristoffers][]) 🎉🎊
- Andre Gilerson ([@AndreGilerson][]) 🎉🎊
- Carl Treudler ([@cajt][])
- Christopher Durand ([@chris-durand][])
- Marten Junga ([@Maju-Ketchup][])
- Michael Thies ([@mhthies][])
- Niklas Hauser ([@salkinium][])
- Raphael Lehmann ([@rleh][])
- Sascha Schade ([@strongly-typed][])

PR [#325][] -> [2017q4][].

<details>
<summary>Detailed changelog</summary>

#### 2018-01-07: Fix HD44780 initialization for 4-bit bus

Adds missing bitshift to write the correct nibble to enable 4-bit mode.

PR [#326][] -> [42837c7][] with low impact on HD44780 driver.  
Tested in hardware by [@acristoffers][].

#### 2018-01-07: Fix GpioPort setOutput()/setInput()/configure() on STM32

Fixes wrong offset of a port mask used to configure output, input and pullups.

PR ? -> [6328b4a][] with low impact on STM32 targets.  
Tested in hardware by [@salkinium][].

#### 2018-01-07: Update CMSIS headers for STM32

PR [#324][] -> [857e514][] with **medium impact** on STM32 targets.  
Tested in hardware by [@salkinium][].

#### 2018-01-03: Use C++ stdlib instead of boost for hosted RTOS

Replaces the hosted implementation of our RTOS interface with the C++ stdlib to reduce our dependence on boost for threading.

PR [#322][] -> [5ef8009][] with low impact on hosted targets.

#### 2018-01-03: Python 3 support for SConscripts and related tools

SCons 3.0 features support for both Python 3.5+ and Python 2.7.
These changes make all build and tool scripts compatible with
both versions and adapt the CI systems too.

PR [#297][] -> [3b47fa5][] with low impact on all build tools.

#### 2017-12-20: Enable C++14 for all targets by default

This is possible since Atmel's offical avr-gcc is now >= v5.4.0.

PR [#320][] -> [68e73b3][] with low impact on all targets.

#### 2017-12-19: Fix matrix multiplication and LU-decomposition

Matrix height and width parameters were switched.

PR [#318][] -> [c4ed672][] with **medium impact** on matrix math.  
Tested in hardware by [@Maju-Ketchup][].

#### 2017-12-18: Fix printing of uint64_t in iostream

Only the lower 32 bits of an uint64_t were shown in iostream.

PR [#315][] -> [2c898ae][] with **medium impact** on printing 64-bit numbers.  
Tested in hardware by [@Maju-Ketchup][].

#### 2017-11-09: Add rosserial to communicate with ROS

Allows using modm for embedded sensor aquisition and interacting with a ROS robot across a serial link via ROS messages.

PR [#306][] -> [78c3ae2][].  
Tested in hardware by [@strongly-typed][].

#### 2017-10-27: Revive LPC11C24 with GPIO, UART and CAN

"out of curiosity"

PR [#305][] -> [48d60e2][] with **medium impact** on LPC targets.  
Tested in hardware by [@strongly-typed][].

#### 2017-10-24: Fix negative temperatures and round integer for LTC298x

PR [#301][] -> [6aee411][] with **medium impact** on negative LTC298x data.  
Tested in hardware by [@rleh][].

#### 2017-10-15: Add CircleCI integration

PR [#298][] -> [c5f8170][].

#### 2017-10-12: Fix the I2C on STM32L4

Correctly generates restart condition on writes > 255 bytes.

PR [#299][] -> [3d9d80f][] with low impact on STM32L0/L4 targets.  
Tested in hardware by [@strongly-typed][].

</details>


## 2017-10-01: 2017q3 release

This release covers everything from the 2017q2 release on 2017-07-02 and has been
tested with avr-gcc v5.4.0 from Atmel and arm-none-eabi-gcc 2017q2 from Arm. 
Note, that Arm did not release 6-2017-q3-update for their toolchain this quarter.

Breaking changes:

- MCP2515 revival adds new `initialize` API

Major features:

- No major features

Major fixes:

- Fix pin remap access (MAPR2 register) for STM32F1 XL Density series
- Fix moving average for negative averages
- Fix printf format string type violations

Known bugs:

- modm may generate separate IRQ handlers for shared interrupts. See [#88][].
- GPIO `connect` on STM32F1 is still broken. See [#178][] for discussion.
  The API from MODM will not be backported to modm however.
- STM32L0/L4 hardware I2C driver has limitations on restart behaviors. See [#255][].
- STM32F107 does not compile due to the HAL trying to remap USB. See [#268][].
- SCons build system emits multiple non-critical warnings. See [#286][].
- SCons 3.0 was released, however, our build system is not fully compatible. See [#293][].

New development board targets:

- OLIMEXINO-STM32 as `olimexino_stm32`
- STM32F051R-DISCO as `stm32f0_discovery`

New device drivers:

- AD79x8
- LTC298x
- AMSYS5915
- MCP2515 (revived)

Many thanks to all our contributors.
A special shoutout to first timers (🎉🎊):

- Carl Treudler ([@cajt][]) 🎉🎊
- Christopher Durand ([@chris-durand][])
- Daniel Krebs ([@daniel-k][])
- Marten Junga ([@Maju-Ketchup][]) 🎉🎊
- Michael Thies ([@mhthies][])
- Niklas Hauser ([@salkinium][])
- Raphael Lehmann ([@rleh][])
- Sascha Schade ([@strongly-typed][])

PR [#296][] -> [2017q3][].

<details>
<summary>Detailed changelog</summary>

#### 2017-10-01: Update CMSIS headers for STM32

PR [#295][] -> [e1f056a][] with **medium impact** on STM32 targets.  
Tested in hardware by [@salkinium][].

#### 2017-09-21: Add OLIMEXINO-STM32 board and example

PR [#288][] -> [9d6620d][].  
Tested in hardware by [@cajt][].

#### 2017-08-24: Add DISCO-F051R8 board and example

PR [#284][] -> [51491ad][].  
Tested in hardware by [@strongly-typed][].

#### 2017-08-22: Revive MCP2515 driver and add 8 MHz config

Tested in hardware with 8 MHz external crystal for
10, 20, 50, 100, 125, 250, 500 and 1000 kBps with
STM32 bxCAN and oscilloscope with protocol decoder.

PR [#278][] -> [b77294e][] with **high impact** on MCP2515 driver users.  
Tested in hardware by [@strongly-typed][].

#### 2017-08-14: Fix moving average for negative averages

Since template parameter N is defined as `std::size_t` which is unsigned,
the result of the average calculation will be implicitly casted and
therefore negative averages gave wrong results.

PR [#272][] -> [e14ba68][] with **medium impact** on filter algorithms.

#### 2017-08-12: Add pressure sensor AMSYS5915 driver

PR [#275][] -> [fc59fc0][].  
Tested in hardware by [@rleh][].

#### 2017-08-12: Add temperature sensor LTC298x driver

PR [#273][] -> [a27ca5d][].  
Tested in hardware by [@rleh][].

#### 2017-08-11: Add ADC AD79x8 driver

PR [#274][] -> [680c92a][].  
Tested in hardware by [@chris-durand][].

#### 2017-07-25: Fix printf format string warnings

PR [#270][] -> [8cc5c78][] with low impact.

#### 2017-07-19: Fix pin remap access for STM32F1

Fix pin remap access (MAPR2 register) for STM32F1 XL Density series

PR [#269][] -> [06b5af9][] with **medium impact** on STM32F1 targets.  
Tested in hardware by [@strongly-typed][].

#### 2017-07-08: Add Python 3 support to MODM xml parser

The MODM (the protocol) tools now work with both Python2 and Python3.

PR [#261][] -> [fc2f33b][] with low impact.

#### 2017-07-04: Check arguments of printf format string

Now the compiler checks the format string for any type violations.

PR [#263][] -> [3f50e1d][] with low impact.

</details>


## 2017-07-02: 2017q2 release

This release covers everything from the 2017q1 release on 2017-04-05 and has been
tested with avr-gcc v5.4.0 from Atmel and arm-none-eabi-gcc 2017q2 from ARM.

Breaking changes:

- No breaking changes

Major features:

- STM32L4 target support and drivers
- Improve assert implementation and make use of them

Major fixes:

- Fix issues and warnings in release tests
- Fix wrong `ns_per_loop` rounding in board definitions

Known bugs:

- modm may generate separate IRQ handlers for shared interrupts. See [#88][].
- GPIO `connect` on STM32F1 is still broken. See [#178][] for discussion.
  The API from MODM will not be backported to modm however.
- STM32L0/L4 hardware I2C driver has limitations on restart behaviors. See [#255][].

New development board targets:

- NUCLEO-L476RG as `nucleo_l476rg`
- STM32L476G-DISCO as `stm32l476_discovery`

New device drivers:

- DS1302
- TCS3472X

Many thanks to all our contributors.
A special shoutout to first timers (🎉🎊):

- Arjun Sarin 🎉🎊
- Michael Thies ([@mhthies][])
- Niklas Hauser ([@salkinium][])
- Sascha Schade ([@strongly-typed][])

PR [#262][] -> [2017q2][].

<details>
<summary>Detailed changelog</summary>

#### 2017-06-28: Fix and improve the release tests

PR [#254][] -> [f2ac1a0][].  
Tested in hardware by [@strongly-typed][].

#### 2017-05-14: Add STM32L4 hardware ADC driver

PR [#249][] -> [29c8905][].  
Tested in hardware by [@strongly-typed][].

#### 2017-05-14: Add STM32L0/L4 hardware I2C driver

PR [#248][] -> [51159ff][].  
Tested in hardware by [@strongly-typed][].

#### 2017-05-13: Add RTC DS1302 driver

PR [#251][] -> [40da657][].  
Tested in hardware by [@strongly-typed][].

#### 2017-05-11: Fix compiler warnings of AVR release tests

PR [#253][] -> [9018741][].  

#### 2017-05-07: Improve implementation, add asserts

- Make header C-compatible so asserts can be called from C.
- Specialize `assert_fail` function for context value.
- Return condition from `modm_assert` for error handling.
- Remove `exit()` calls from implementation.
- Add assertions to core.
- Update F469-DISCO assert example.

PR [#247][] -> [3992534][] with low impact.  
Tested in hardware by [@salkinium][].

#### 2017-05-02: Add platform support for STM32L4 family

- Updates the DFG and SCons for STM32L4 family.
- Fixes AF issues with TimerN BreakIn signal.
- Adds ports of drivers for STM32L4 family.
- Adds BSPs for STM32L476 Nucleo and Discovery.
- Adds examples for STM32L476 Nucleo and Discovery.

PR [#240][] -> [e9591d5][] with **medium impact** on STM32 targets.  
Tested in hardware by [@strongly-typed][].

#### 2017-04-29: Implement %p in printf

PR [#246][] -> [a906c2d][].

#### 2017-04-27: Add TCS3472X color sensor driver

PR [#244][] -> [68b904e][].

#### 2017-04-27: Fix UART for F0x0 series.

UART does not have LIN or Smartcard mode.

PR [#243][] -> [7111cd3][].

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
- ZeroMQ as transport layer for MODM
- Update STM32 header and device files
- Namespace architecture/util.hpp macros
- Coding conventions
- Porting guide

Major fixes:

- Fix implicit conversion of Flags
- Fix I2C master reception of 2B transfers
- Fix spelling of `SpiMaster::acquire`

Known bugs:

- modm may generate separate IRQ handlers for shared interrupts. See [#88][].
- GPIO `connect` on STM32F1 is broken. They can be remapped only in groups,
  however, the API allows invalid remapping. This cannot be fixed without
  introducing a new API for that. See [#178][] for discussion.
  A solution has been tested for modm, but isn't ready for modm.

New development board targets:

- NUCLEO-F031K6 as `nucleo_f031k6`
- NUCLEO-F303K8 as `nucleo_f303k8`
- NUCLEO-F401RE as `nucleo_f401re`
- NUCLEO-F411RE as `nucleo_f411re`
- NUCLEO-F429ZI as `nucleo_f429zi`
- STM32F769I-DISCO as `stm32f769i_discovery`
- STM32F469I-DISCO as `stm32f469_discovery`
- STM32F103 board "Blue Pill" as `stm32f103c8t6_blue_pill`

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

#### 2016-12-10: Require Identifier for MODM containers

In the progress of adding raw Ethernet frames as a backend for MODM
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

- `MODM_ISR(vector, attributes...)` for declaring a static ISR with
                                    attributes (e.g. `modm_fastcode`).
- `MODM_ISR_NAME(vector)` for getting the platform specific ISR name.
- `MODM_ISR_DECL(vector)` for forward declarations of ISRs.
- `MODM_ISR_CALL(vector)` for calling ISRs manually from code.

`MODM_ISR_NAME(vector)` is backwards compatible, mapping to
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

For details see: http://blog.salkinium.com/modms-error-model

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

#### 2016-10-03: Add ZeroMQ as transport layer for MODM and example

This feature adds a ZeroMQ implementation of the MODM backend as a
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

The `modm/architecture/util.hpp` macros have been renamed and
properly documented.
A new header `modm/architecture/legacy_macros.hpp` contains backwards
compatible mappings for application code. All occurrences of the non-
namespaced macros in modm have been replaced. The backwards-compatible
mapping can be disabled by declaring the `MODM_DISABLE_LEGACY_MACROS`
macro to the compiler.

- ALWAYS_INLINE        -> modm_always_inline
- ATTRIBUTE_UNUSED     -> modm_unused
- ATTRIBUTE_WEAK       -> modm_weak
- ATTRIBUTE_ALIGNED(n) -> modm_aligned(n)
- ATTRIBUTE_PACKED     -> modm_packed
- ATTRIBUTE_FASTCODE   -> modm_fastcode
- ATTRIBUTE_FASTDATA   -> modm_fastdata
- ATTRIBUTE_MAY_ALIAS  -> modm_may_alias
- CONCAT(a,b)          -> MODM_CONCAT(a,b)
- STRINGIFY(a)         -> MODM_STRINGIFY(a)
- MOD_RRAY_SIZE(x)  -> MODM_ARRAY_SIZE(x)
- likely(x)            -> modm_likely(x)
- unlikely(x)          -> modm_unlikely(x)

Note that the `ENUM_CLASS_FLAG` was replaced with `modm::Flags32` and
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

Three heap algorithms can be chosen from using the modm parameters:

- newlib's dlmalloc (default): Chooses largest continuous heap from page table.
- modm's block allocator: Chooses largest heap from page table.
- TLSF: Uses all heaps in the table.

Overloading of the new operator in C++ is provided:
```cpp
Foo *foo = new (modm::MemoryFastData) Foo(args);
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

This replaces the very dynamic `modm::stm32::SystemClock` with a fake
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
[2017q1]: https://github.com/roboterclubaachen/modm/releases/tag/2017q1
[2017q2]: https://github.com/roboterclubaachen/modm/releases/tag/2017q2
[2017q3]: https://github.com/roboterclubaachen/modm/releases/tag/2017q3
[2017q4]: https://github.com/roboterclubaachen/modm/releases/tag/2017q4

<!-- Contributors -->
[@7Kronos]: https://github.com/7Kronos
[@acristoffers]: https://github.com/acristoffers
[@AndreGilerson]: https://github.com/AndreGilerson
[@cajt]: https://github.com/cajt
[@chris-durand]: https://github.com/chris-durand
[@daniel-k]: https://github.com/daniel-k
[@dergraaf]: https://github.com/dergraaf
[@ekiwi]: https://github.com/ekiwi
[@genbattle]: https://github.com/genbattle
[@georgi-g]: https://github.com/georgi-g
[@Maju-Ketchup]: https://github.com/Maju-Ketchup
[@mhthies]: https://github.com/mhthies
[@rleh]: https://github.com/rleh
[@salkinium]: https://github.com/salkinium
[@Sh4rK]: https://github.com/Sh4rK
[@strongly-typed]: https://github.com/strongly-typed
[@tomchy]: https://github.com/tomchy

<!-- Pull requests or Issues -->
[#88]: https://github.com/roboterclubaachen/modm/pull/88
[#115]: https://github.com/roboterclubaachen/modm/pull/115
[#129]: https://github.com/roboterclubaachen/modm/pull/129
[#137]: https://github.com/roboterclubaachen/modm/pull/137
[#138]: https://github.com/roboterclubaachen/modm/pull/138
[#143]: https://github.com/roboterclubaachen/modm/pull/143
[#144]: https://github.com/roboterclubaachen/modm/pull/144
[#145]: https://github.com/roboterclubaachen/modm/pull/145
[#147]: https://github.com/roboterclubaachen/modm/pull/147
[#154]: https://github.com/roboterclubaachen/modm/pull/154
[#155]: https://github.com/roboterclubaachen/modm/pull/155
[#165]: https://github.com/roboterclubaachen/modm/pull/165
[#167]: https://github.com/roboterclubaachen/modm/pull/167
[#168]: https://github.com/roboterclubaachen/modm/pull/168
[#173]: https://github.com/roboterclubaachen/modm/pull/173
[#175]: https://github.com/roboterclubaachen/modm/pull/175
[#176]: https://github.com/roboterclubaachen/modm/pull/176
[#178]: https://github.com/roboterclubaachen/modm/pull/178
[#179]: https://github.com/roboterclubaachen/modm/pull/179
[#180]: https://github.com/roboterclubaachen/modm/pull/180
[#182]: https://github.com/roboterclubaachen/modm/pull/182
[#183]: https://github.com/roboterclubaachen/modm/pull/183
[#185]: https://github.com/roboterclubaachen/modm/pull/185
[#186]: https://github.com/roboterclubaachen/modm/pull/186
[#187]: https://github.com/roboterclubaachen/modm/pull/187
[#188]: https://github.com/roboterclubaachen/modm/pull/188
[#189]: https://github.com/roboterclubaachen/modm/pull/189
[#194]: https://github.com/roboterclubaachen/modm/pull/194
[#195]: https://github.com/roboterclubaachen/modm/pull/195
[#196]: https://github.com/roboterclubaachen/modm/pull/196
[#202]: https://github.com/roboterclubaachen/modm/pull/202
[#204]: https://github.com/roboterclubaachen/modm/pull/204
[#221]: https://github.com/roboterclubaachen/modm/pull/221
[#224]: https://github.com/roboterclubaachen/modm/pull/224
[#226]: https://github.com/roboterclubaachen/modm/pull/226
[#229]: https://github.com/roboterclubaachen/modm/pull/229
[#230]: https://github.com/roboterclubaachen/modm/pull/230
[#231]: https://github.com/roboterclubaachen/modm/pull/231
[#234]: https://github.com/roboterclubaachen/modm/pull/234
[#235]: https://github.com/roboterclubaachen/modm/pull/235
[#237]: https://github.com/roboterclubaachen/modm/pull/237
[#240]: https://github.com/roboterclubaachen/modm/pull/240
[#243]: https://github.com/roboterclubaachen/modm/pull/243
[#244]: https://github.com/roboterclubaachen/modm/pull/244
[#246]: https://github.com/roboterclubaachen/modm/pull/246
[#247]: https://github.com/roboterclubaachen/modm/pull/247
[#248]: https://github.com/roboterclubaachen/modm/pull/248
[#249]: https://github.com/roboterclubaachen/modm/pull/249
[#251]: https://github.com/roboterclubaachen/modm/pull/251
[#253]: https://github.com/roboterclubaachen/modm/pull/253
[#254]: https://github.com/roboterclubaachen/modm/pull/254
[#255]: https://github.com/roboterclubaachen/modm/pull/255
[#261]: https://github.com/roboterclubaachen/modm/pull/261
[#262]: https://github.com/roboterclubaachen/modm/pull/262
[#263]: https://github.com/roboterclubaachen/modm/pull/263
[#268]: https://github.com/roboterclubaachen/modm/pull/268
[#269]: https://github.com/roboterclubaachen/modm/pull/269
[#270]: https://github.com/roboterclubaachen/modm/pull/270
[#272]: https://github.com/roboterclubaachen/modm/pull/272
[#273]: https://github.com/roboterclubaachen/modm/pull/273
[#274]: https://github.com/roboterclubaachen/modm/pull/274
[#275]: https://github.com/roboterclubaachen/modm/pull/275
[#278]: https://github.com/roboterclubaachen/modm/pull/278
[#284]: https://github.com/roboterclubaachen/modm/pull/284
[#286]: https://github.com/roboterclubaachen/modm/pull/286
[#288]: https://github.com/roboterclubaachen/modm/pull/288
[#293]: https://github.com/roboterclubaachen/modm/pull/293
[#295]: https://github.com/roboterclubaachen/modm/pull/295
[#296]: https://github.com/roboterclubaachen/modm/pull/296
[#297]: https://github.com/roboterclubaachen/modm/pull/297
[#298]: https://github.com/roboterclubaachen/modm/pull/298
[#299]: https://github.com/roboterclubaachen/modm/pull/299
[#301]: https://github.com/roboterclubaachen/modm/pull/301
[#305]: https://github.com/roboterclubaachen/modm/pull/305
[#306]: https://github.com/roboterclubaachen/modm/pull/306
[#314]: https://github.com/roboterclubaachen/modm/pull/314
[#315]: https://github.com/roboterclubaachen/modm/pull/315
[#318]: https://github.com/roboterclubaachen/modm/pull/318
[#320]: https://github.com/roboterclubaachen/modm/pull/320
[#322]: https://github.com/roboterclubaachen/modm/pull/322
[#324]: https://github.com/roboterclubaachen/modm/pull/324
[#325]: https://github.com/roboterclubaachen/modm/pull/325
[#326]: https://github.com/roboterclubaachen/modm/pull/326

<!-- Commits -->
[0118a13]: https://github.com/roboterclubaachen/modm/commit/0118a13
[06b5af9]: https://github.com/roboterclubaachen/modm/commit/06b5af9
[08784cd]: https://github.com/roboterclubaachen/modm/commit/08784cd
[0dbf73c]: https://github.com/roboterclubaachen/modm/commit/0dbf73c
[0dbf73c]: https://github.com/roboterclubaachen/modm/commit/0dbf73c
[0e7e57f]: https://github.com/roboterclubaachen/modm/commit/0e7e57f
[13904f8]: https://github.com/roboterclubaachen/modm/commit/13904f8
[13904f8]: https://github.com/roboterclubaachen/modm/commit/13904f8
[170f53a]: https://github.com/roboterclubaachen/modm/commit/170f53a
[2504682]: https://github.com/roboterclubaachen/modm/commit/2504682
[26471ab]: https://github.com/roboterclubaachen/modm/commit/26471ab
[29c8905]: https://github.com/roboterclubaachen/modm/commit/29c8905
[2c898ae]: https://github.com/roboterclubaachen/modm/commit/2c898ae
[3992534]: https://github.com/roboterclubaachen/modm/commit/3992534
[3b47fa5]: https://github.com/roboterclubaachen/modm/commit/3b47fa5
[3c7cd31]: https://github.com/roboterclubaachen/modm/commit/3c7cd31
[3d9d80f]: https://github.com/roboterclubaachen/modm/commit/3d9d80f
[3f50e1d]: https://github.com/roboterclubaachen/modm/commit/3f50e1d
[408c309]: https://github.com/roboterclubaachen/modm/commit/408c309
[40da657]: https://github.com/roboterclubaachen/modm/commit/40da657
[41ab22a]: https://github.com/roboterclubaachen/modm/commit/41ab22a
[42837c7]: https://github.com/roboterclubaachen/modm/commit/42837c7
[48d60e2]: https://github.com/roboterclubaachen/modm/commit/48d60e2
[51159ff]: https://github.com/roboterclubaachen/modm/commit/51159ff
[51491ad]: https://github.com/roboterclubaachen/modm/commit/51491ad
[553dceb]: https://github.com/roboterclubaachen/modm/commit/553dceb
[5e547ab]: https://github.com/roboterclubaachen/modm/commit/5e547ab
[5ef8009]: https://github.com/roboterclubaachen/modm/commit/5ef8009
[5f5934a]: https://github.com/roboterclubaachen/modm/commit/5f5934a
[6328b4a]: https://github.com/roboterclubaachen/modm/commit/6328b4a
[637e074]: https://github.com/roboterclubaachen/modm/commit/637e074
[63ad1d3]: https://github.com/roboterclubaachen/modm/commit/63ad1d3
[680c92a]: https://github.com/roboterclubaachen/modm/commit/680c92a
[68b904e]: https://github.com/roboterclubaachen/modm/commit/68b904e
[68e73b3]: https://github.com/roboterclubaachen/modm/commit/68e73b3
[6aee411]: https://github.com/roboterclubaachen/modm/commit/6aee411
[6c1a111]: https://github.com/roboterclubaachen/modm/commit/6c1a111
[7111cd3]: https://github.com/roboterclubaachen/modm/commit/7111cd3
[78c3ae2]: https://github.com/roboterclubaachen/modm/commit/78c3ae2
[7ab0132]: https://github.com/roboterclubaachen/modm/commit/7ab0132
[84d5bd0]: https://github.com/roboterclubaachen/modm/commit/84d5bd0
[857e514]: https://github.com/roboterclubaachen/modm/commit/857e514
[8cc5c78]: https://github.com/roboterclubaachen/modm/commit/8cc5c78
[8f9b154]: https://github.com/roboterclubaachen/modm/commit/8f9b154
[9018741]: https://github.com/roboterclubaachen/modm/commit/9018741
[967c0a9]: https://github.com/roboterclubaachen/modm/commit/967c0a9
[9940a65]: https://github.com/roboterclubaachen/modm/commit/9940a65
[9d6620d]: https://github.com/roboterclubaachen/modm/commit/9d6620d
[a00d3cc]: https://github.com/roboterclubaachen/modm/commit/a00d3cc
[a27ca5d]: https://github.com/roboterclubaachen/modm/commit/a27ca5d
[a379e61]: https://github.com/roboterclubaachen/modm/commit/a379e61
[a6519c3]: https://github.com/roboterclubaachen/modm/commit/a6519c3
[a8a2322]: https://github.com/roboterclubaachen/modm/commit/a8a2322
[a906c2d]: https://github.com/roboterclubaachen/modm/commit/a906c2d
[b21f502]: https://github.com/roboterclubaachen/modm/commit/b21f502
[b77294e]: https://github.com/roboterclubaachen/modm/commit/b77294e
[bb3fa3a]: https://github.com/roboterclubaachen/modm/commit/bb3fa3a
[c12a69b]: https://github.com/roboterclubaachen/modm/commit/c12a69b
[c4ed672]: https://github.com/roboterclubaachen/modm/commit/c4ed672
[c5f8170]: https://github.com/roboterclubaachen/modm/commit/c5f8170
[c605416]: https://github.com/roboterclubaachen/modm/commit/c605416
[c7adb48]: https://github.com/roboterclubaachen/modm/commit/c7adb48
[cb0a11e]: https://github.com/roboterclubaachen/modm/commit/cb0a11e
[d949fee]: https://github.com/roboterclubaachen/modm/commit/d949fee
[da784bd]: https://github.com/roboterclubaachen/modm/commit/da784bd
[dd3639b]: https://github.com/roboterclubaachen/modm/commit/dd3639b
[e14ba68]: https://github.com/roboterclubaachen/modm/commit/e14ba68
[e1efaf4]: https://github.com/roboterclubaachen/modm/commit/e1efaf4
[e1f056a]: https://github.com/roboterclubaachen/modm/commit/e1f056a
[e2f9b4a]: https://github.com/roboterclubaachen/modm/commit/e2f9b4a
[e346020]: https://github.com/roboterclubaachen/modm/commit/e346020
[e9591d5]: https://github.com/roboterclubaachen/modm/commit/e9591d5
[f2ac1a0]: https://github.com/roboterclubaachen/modm/commit/f2ac1a0
[f472f7f]: https://github.com/roboterclubaachen/modm/commit/f472f7f
[f780c2a]: https://github.com/roboterclubaachen/modm/commit/f780c2a
[fc2f33b]: https://github.com/roboterclubaachen/modm/commit/fc2f33b
[fc59fc0]: https://github.com/roboterclubaachen/modm/commit/fc59fc0
[fcf27a1]: https://github.com/roboterclubaachen/modm/commit/fcf27a1
[fd1b109]: https://github.com/roboterclubaachen/modm/commit/fd1b109
[ffa4e1b]: https://github.com/roboterclubaachen/modm/commit/ffa4e1b
