# Color

Color containers and converters in various formats: RGB, HSV, Brightness, Rgb565. You can 'invent' your own types by choosing the digits used for each component use template arguments.

## Construction and Assigment
The types are quite flexible and thanks to constexpr, the following constructions of red are resolved at compile time.

All following assignments create the same plain Red: #FF0000, Rgb(255, 0, 0)

Declare a RGB color with 8bits per channel
```cpp
#include <modm/ui/color.hpp>

modm::color::Rgb888 red; // Rgb888 is shorthand for Rgb<8, 8, 8>
```

Obligatory construction
```cpp
red = modm::color::Rgb888(255, 0, 0);
```

modm supports html named colors
```cpp
red = modm::color::html::Red;
```

Changing resolution results in scaled values

Here, the red-component has just **3** bits and a value of **7** makes it **full saturated**.
After conversion to Rgb<**8**, 8, 8> it's still **fully saturated**.
Red now equals **255**.
```cpp
red = modm::color::Rgb<3, 2, 1>(7, 0, 0);
```

Convert from different ColorType
```cpp
red = modm::color::Hsv888(0, 255, 255);
```

Colors components are essentially color::Gray<>
This makes plain red as well
```cpp
modm::color::Gray2 white(3);
modm::color::Gray2 black(0);

red = (white, black, black);
```

This is a conversion from Gray to Rgb.
It mixes white in rgb colorspace
```cpp
red = white; // -> Rgb888(255, 255, 255)
```
## Saturation Arithmetics

Every arithmetic operations yields in burned out or underexposed colors.

Only the Hue component of Hsv ColorType wraps around just like integers do.

## Flexible Widgets
- [ ] Complete this snippet and make the code actually working

The flexible colortypes open the doors for flexible UI components as well.
You can write one codebase that has all the supports the cheapest  to full color TFTs:

```cpp
#include <modm/ui/color.hpp>

template<modm::color::Color C>
drawAlertButton() {
	// @warning: Pseudocode. These methods do not yet exist!
	drawFilledRectangle( static_cast<C>(Hsv888(0, 40, 200)) ); // Button Background
	drawRectangle( static_cast<C>(modm::color::DarkRed) ); // Button Border
	drawText( static_cast<C>(modm::color::DarkRed), "Alert, Alert!"); // Button Text
}

// Draws a red alert button
drawAlertButton<modm::color::Rgb888>();

// Draws a similar red alert button
drawAlertButton<modm::color::Rgb565>();

// Also draws a similar red alert button
drawAlertButton<modm::color::Hsv888>();

// In Monochrome colorspace at least you get the Button
drawAlertButton<modm::color::Monochrome>(); // Monochrome = shorthand for Gray<1>

```