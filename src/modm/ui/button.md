# Debouncing Buttons

The `modm::ButtonGroup` class is able to debounce eight buttons at the same time.
The buttons have to be low-active. If this isn't the case invert their signal
before passing it to the `update()` method.

The `update()` method needs to be called periodically for example every 10ms.
Preferred in a timer interrupt function.

The detection for long or repeated presses works only correctly for one key at a
time. This constraint only applies to buttons listed in the `mask` variable.

## Mode 1
```
                 Timeline ---->
                      __      _________________      __
getState()       ____/  \____/                 \____/  \____
isPressed()      ----X-------X----------------------X-------
isRepeated()     --------------------X--X--X--X-------------
isReleased()     -------X----------------------X-------X----
                             |       |__|__|
                             |_______| \ /
                                  \    interval
                                  timeout
```

## Mode 2
```
                 Timeline ---->
                      __      _________________      __
getState()       ____/  \____/                 \____/  \____
isPressedShort() -------X------------------------------X----
isPressedLong()  --------------------X----------------------
isReleased()     -------X----------------------X-------X----
```

## Naming Buttons

To name buttons, declare an enum with a bitmask for each button:

```cpp
#include <modm/math/utils/bit_constants.hpp>

enum ButtonIdentifier
{
	NONE = 0x00,
	BUTTON0 = modm::Bit0,
	BUTTON1 = modm::Bit1,
	BUTTON2 = modm::Bit2,
	BUTTON3 = modm::Bit3,
	BUTTON4 = modm::Bit4,
	BUTTON5 = modm::Bit5,
	BUTTON6 = modm::Bit6,
	BUTTON7 = modm::Bit7,
	ALL = 0xFF,
};
```

Pass a `ButtonIdentifier` to any of `ButtonGroup::is**()` like so

```cpp
if(buttongroup_instance.isPressed(BUTTON0)) {
	// Do stuff
}
```

This implementation is based on the C functions written by Peter Dannegger
(see http://www.mikrocontroller.net/topic/48465).