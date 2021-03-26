# STM32 CMSIS Headers

This module wraps [the CMSIS headers for STM32 devices][repo] and provides the
header file and CPP definitions for accessing the `modm:target` peripherals.
Specifically, it copies the target header file which provides the CMSIS
implementation of the target's language interface for its memory-mapped
peripherals.

!!! bug "Discrepancies between documentation and headers"
    These CMSIS headers should reflect what's written in the documentation,
    however, in our experience there may be small differences in naming,
    behavior and availability between it and this implementation.

The generated header file `modm/platform/device.hpp` contains the necessary
target define `STM32{FAMILY}{NAME}xx` or similar and includes the
`modm:target`-specific header.

!!! tip "Include only `modm/platform/device.hpp`!"
	This file is only included by interface implementations inside the
	`modm/platform` folder! If you need to implement your own drivers, include
	only this file.


## CMSIS Compliance

modm implements the target specific CMSIS functionality itself, without using
the CMSIS interface. This is necessary to provide similar functionality on
non-Cortex-M based targets.

- `uint32_t SystemCoreClock` contains an up-to-date value of CPU frequency.
- `void SystemCoreClockUpdate()` does nothing, it's a weak empty function.
- `void SystemInit()` is **undefined**: modm defines its own startup
  architecture, see the `modm:platform:core` module for details.


[repo]: https://github.com/modm-io/cmsis-header-stm32
[patches]: https://github.com/modm-io/cmsis-header-stm32/tree/master/patches


## Debugging Peripherals

When debugging with GDB, it can sometimes be difficult to access peripherals,
due to the CMSIS implementation with CPP defines and GCC optimization flags.
Depending on these circumstances, GDB may only "see" a peripheral in a certain
context, which can make the debugging experience quite frustrating.

To aid this, we generate a linkerscript fragment and source file, which place
the peripherals memory as real objects in the peripheral memory space.
You can then access these peripherals inside GDB at any time, regardless of
context and build profile setting:

```
(gdb) p/x *GPIOB
$1 = {
  MODER = 0xaa0280,
  OTYPER = 0x300,
  OSPEEDR = 0x2a00c0,
  PUPDR = 0x400100,
  IDR = 0xfd0,
  ODR = 0x100,
  BSRR = 0x0,
  LCKR = 0x0,
  AFR = {0x0, 0x7744}
}
```

**This does not have any effect on your firmware!** It is purely a debug
helper feature and none of these definitions make it into the executable.

!!! tip "Beware read/write side-effects"
    When debugging be aware of the side-effects that your read or write to a
    peripheral memory location can have. It's fairly obvious that a write may
    have consequences, but also reads can modify peripheral state, for example,
    a hardware FIFO buffer may pop the queue if your read the top of it.
