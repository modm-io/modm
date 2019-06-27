# LED Animation and Gamma Correction

Header: `#include <modm/ui/led.hpp>`

This module provides abstractions for animating LEDs by wrapping the
*modm:ui:animation* module and providing look-up tables for performing
gamma correction of LED brightness.

The main functionality is part of the `modm::ui::Led` class, which provides
a basic interface to fade an LED with an 8-bit value.
Note that this class does *not* do any gamma correction on it's own, it just
wraps an 8-bit `modm::ui::Animation` and a 8-bit value.

You must provide a function handler which gets called whenever the LED value
needs updating, at most every 1ms, but only when the value has actually changed.
The implementation of this function is up to you.

<!--
.. compile::
    :group: general
-->

```cpp
void led_handler(uint8_t brightness)
{
    // LED control implementation = brightness
}
modm::ui::Led led_function(led_handler);
// You may also use a lambda function for brevity
modm::ui::Led led_lambda([](uint8_t brightness)
{
    // LED control implementation = brightness
});
```

## Animating LEDs

To animate the LED you need to call the `modm::ui::Led::update()` member
function regularly.
The function updates the LED value at most every 1ms, however you may call it
more often than that. If the function is called less regularly, it will
automatically catch up the skipped milliseconds, and execute the brightness
handler at most once if the value has changed.
Depending on how smooth you require your animation to be, you may call the
update function with intervals of up to 250ms.

You can also pass the `modm::ui::Led` class directly into the
`modm::ui::KeyFrameAnimation` class and it's derivatives, to animate it's
brightness by interpolating between key frames.
Note that you only need to call the `update()` member of the animator!

<!--
.. compile::
    :group: general
    :include: <modm/ui/animation.hpp>
-->

```cpp
modm::ui::Led led;
modm::ui::Indicator<uint8_t> indicator(led);
indicator.start();

while (true)
{
    indicator.update();
}
```

If you want to animate a lot of LEDs it is more efficient to execute all update
methods inside a 1ms `modm::PeriodicTimer`:

<!--
.. compile::
    :group: general
    :prefix: |
        modm::ui::Led led1;
        modm::ui::Led led2;
        modm::ui::Led ledN;
-->

```cpp
modm::PeriodicTimer update_timer(1);

while (true)
{
    if (update_timer.execute())
    {
        led1.update();
        led2.update();
        // ...
        ledN.update();
    }
}
```


## Using Gamma Correction

In order to map the linearly animated brightness value to a gamma-corrected
value, this module by default generates look-up tables for a gamma value of 2.2
mapping from a 256 input range to 7, 8, 10, 12 and 16 bit resolution output
ranges. The generated tables have this naming schema:
`modm::ui::table{gamma}_{output_bits}_{input_range}`.

For example, you can set up a timer to output 16-bit PWM to drive your LED, and
use the `modm::ui::table22_16_256` table to map the linear LED animation values
to a gamma corrected 16-bit value:

<!--
.. compile::
    :group: timer4
-->

```cpp
modm::ui::Led led_timer([](uint8_t brightness)
{
    Timer4::setCompareValue(1, modm::ui::table22_16_256[brightness]);
});
```

You may also generate other tables using the module options, for example, if
you want to map a percentage (0-100%) to LED brightness, without scaling this
to the full 256 value range, you can also generate a 100 value look-up table:

```xml
<option name="modm:ui:led:range">100,256</option>
<option name="modm:ui:led:bit">8,16</option>
```

This module generated the product of these options, in this case 4 look-up
tables. The linker will throw out those you don't use.

```cpp
namespace modm::ui
{
    /// 2.2 gamma correction with 8-bit resolution (0 to 255) in 100 steps at 100 bytes.
    extern modm::accessor::Flash<uint8_t> table22_8_100;
    /// 2.2 gamma correction with 8-bit resolution (0 to 255) in 256 steps at 256 bytes.
    extern modm::accessor::Flash<uint8_t> table22_8_256;
    /// 2.2 gamma correction with 16-bit resolution (0 to 65535) in 100 steps at 200 bytes.
    extern modm::accessor::Flash<uint16_t> table22_16_100;
    /// 2.2 gamma correction with 16-bit resolution (0 to 65535) in 256 steps at 512 bytes.
    extern modm::accessor::Flash<uint16_t> table22_16_256;
}
```

You can then animate your LED in the 0-99 value range:

<!--
.. compile::
    :group: timer4
-->

```cpp
modm::ui::Led led_percentage([](uint8_t brightness)
{
    if (brightness >= 100) brightness = 99;
    Timer4::setCompareValue(1, modm::ui::table22_16_100[brightness]);
});
uint8_t percent = 50;
// fade to 50% brightness within 1 second
led_percentage.fadeTo(percent, 1000);
```

Note that you can also use the handler to animate LEDs which are connected
externally, for example, via a WS2812 LED chain.
Here you can use a helper function to map the values into an array which then
transferred into the WS2812 LED chain.

<!--
.. compile::
    :group: general
-->

```cpp
uint8_t ws2812_values[3];
void ws2812_setter(uint8_t index, uint8_t brightness)
{
    ws2812_values[index] = modm::ui::table22_8_256[brightness];
}
// Construct an array of LED animators
modm::ui::Led leds[3] =
{
    { [](uint8_t b) { ws2812_setter(0, b); } },
    { [](uint8_t b) { ws2812_setter(1, b); } },
    { [](uint8_t b) { ws2812_setter(2, b); } },
};
// Group them together as one RGB LED
modm::ui::RgbLed rgb(leds[1], leds[0], leds[2]);
// animate to orange within 2 seconds
rgb.fadeTo(modm::ui::Rgb(95, 177, 147), 2000);
```


<!--
.. group::
    :name: general
    :target: *
    :include: <modm/ui/led.hpp>
    :option: <option name="modm:ui:led:range">100,256</option>
    :option: <option name="modm:ui:led:bit">8,16</option>
-->

<!--
.. group::
    :name: timer4
    :target: stm32*
    :module: modm:platform:timer:4
    :include: <modm/ui/led.hpp>
    :option: <option name="modm:ui:led:range">100,256</option>
    :option: <option name="modm:ui:led:bit">8,16</option>
-->
