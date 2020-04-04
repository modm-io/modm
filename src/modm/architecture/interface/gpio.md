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

