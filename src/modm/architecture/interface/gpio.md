# General Purpose Input/Output

GPIOs are modeled as individual classes with an interface consisting out of a
common set of static methods.

## Using GPIOs

These classes are used to create architecture independent
definitions for hardware pins which then can be used as template
parameters for miscellaneous device drivers.

Example of a platform-independent blinking light:

<!--
.. compile::
    :target: *
-->

```cpp
#include <modm/platform.hpp>

using Led = GpioOutputB0;

Led::setOutput();
Led::set();

while (true)
{
    Led::toggle();
    modm::delay(500ms);
}
```

You can use the common definitions of `modm::Gpio` to express your intention
for setting a pin. However, you can also use anything that casts to `bool`.

<!--
.. compile::
    :target: *
    :prefix: |
    	using Led = GpioOutputB0;
-->

```cpp
Led::set(modm::Gpio::High); // Turns Led on
Led::set(modm::Gpio::Low); // Turns Led off
Led::set(true);
Led::set(false);
Led::set(1);
Led::set(0);
Led::set();
Led::reset();
```

!!! warning "Initialize your GPIOs"
    Call `Gpio::setInput()` or `Gpio::setOutput()` before the first use,
    otherwise the GPIO state may be undefined!

See the `modm:platform:gpio` module for platform-specific GPIO behaviors,
especially signal connections to peripherals.


## Runtime GPIOs

The static GPIO classes are as fast as possible, but every pin is a separate
type. When a pin is only known at runtime, for example because the wiring
depends on the hardware revision, or the same code is used with several pins,
use one of the runtime pins. They are values of 8 bytes that can be copied and
stored in arrays and `constexpr` tables:

- `modm::platform::GpioPin`: a pin of this device, also an inverted one, as
  port address and BSRR bits, almost as fast as the static classes.
- `modm::AnyGpio`: a pin of this device or unused. Device pins take the inlined
  fast path plus one test and branch, and unused pins do nothing like
  `GpioUnused`, also when default constructed.

```cpp
GpioPin led = GpioInverted<GpioA5>{};
static constexpr modm::AnyGpio pins[]{GpioA0{}, GpioPin{Gpio::Port::B, 3}, GpioUnused{}};
pins[index].toggle();
```

Copy an `AnyGpio` into a local variable in hot loops: the compiler can then move
the test out of the loop and the pin is as fast as `GpioPin`.

`modm::AnyGpioAdapter<Source>` is a static GPIO class that forwards to the
`AnyGpio` returned by `Source::get()`, for classes that take their pins as
template arguments, like `BitBangI2cMaster`:

```cpp
struct SclSource
{
    static modm::AnyGpio get() { return table[hardware_revision()]; }
};
using Scl = modm::AnyGpioAdapter<SclSource>;
```

Runtime pins are only implemented for STM32 so far.
