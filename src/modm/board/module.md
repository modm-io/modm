# Board Support Packages

modm provides pre-configured BSPs for several commercial off-the-shelf
development boards. There are two main components to the BPSs:

1. An inheritable configuration `board.xml` containing the HAL target, options
   and the board module. This pre-defined configuration is aliased as a repo
   configuration so that your `project.xml` simply `<extends>modm:{board-name}</extends>`.
2. A module `modm:board:{board-name}` that pulls in required dependencies,
   configures the modm library and provides the code to initialize the board.
   You can then `#include <modm/board.hpp>` in your project.

The BSPs all use a common interface within a top-level `namespace Board`:

- `Board::initialize()`: Initializes the targets clock system, logger, LEDs and
  Buttons.
- `Board::initialize{subsystem}()`: Initializes optional board subsystems.
- `Board::SystemClock`: Provides the clock configuration for use in
  `Peripheral::initialize<Board::SystemClock, ...>()`.
- `Board::Led{name}`: Board-specific LEDs  are initialized as outputs and off.
- `Board::Leds`: A `modm::platform::SoftwareGpioPort` containing all board LEDs.
- `Board::Button`: Board-specific input buttons are initialized as input with
  pull-up/down as required.
- `Board::{pin-name}`: All board-specific pins are aliased to their respective
  `modm::platform::Gpio{port}{pin}`.

If the board supports a dedicated serial logging output the BSP redirects the
`modm:debug` module debug stream `MODM_LOG_INFO` etc.

Please note that YOU must explicitly call the `Board` functions to initialize
your hardware, just including the board module is not enough.
Here is an example using the `modm:disco-f469ni` BSP:

```cpp
#include <modm/board.hpp>

int main()
{
	// ALWAYS initialize the board first!
	Board::initialize();
	// Then initialize the subsystems you want to use
	Board::initializeDisplay();

	// Set LEDs via the GPIO port
	Board::Leds::write(0b1011);

	// Use the Arduino pin names
	Board::D0::setOutput(modm::Gpio::High);
	Board::D1::setInput();

	// Use the boards serial logging
	MODM_LOG_INFO << "REBOOT!" << modm::endl;

	while (true) {
		// Link the LED to the button
		Board::LedBlue::set(Board::Button:read());
	}
	return 0;
}
```

!!! warning "Only select one BSP module"
	Even though some targets have multiple BSPs modules available (for example:
	Blue Pill and Black Pill), you can only use one module, since all define the
	same functions resulting in naming conflicts.


## Customization

The BSPs contain an opinionated set of pre-defined functionality and settings.
This is great for just quickly prototyping something, however, when you want to
use custom hardware, or even just change a few settings, it's better to use your
own BSP:

1. Generate the BSP closest to your custom hardware, then copy the files from
   `modm/src/modm/board/{name}` to your own project and modify them.
2. In your `project.xml` remove the board config inheritance (`<extends>`) and
   instead copy the pre-defined options into your own config.
3. Check what modm modules you need to depend on and add them to your own project
   (check for `module.depends(...)` in the BSPs `module.lb`).
4. You may need to manually add the pre-defined collector values to your project
   configuration (check for `env.collect(...)` in the BSPs `module.lb`).


## Create the SystemClock struct

The easiest way using ST's CubeMX tool.

### 1. CubeMX Clock Graph

First we create a project in CubeMX with the desired microcontroller using the
largest (pin-count, flash) variant. CubeMX displays something like this in the
"Clock configuration" tab:

![ST CubeMX Clock Configuration example](https://user-images.githubusercontent.com/2820734/81305132-43715000-907e-11ea-8b8e-b444dc7560cf.png)

Then configure all clocks, muxes, multipliers and dividers to the highest
allowed clock speeds (*).

(*) exceptions: E.g. USB usually requires exactly 48 MHz.

This settings are reflected in the constants `static constexpr uint32_t Frequency`,
`Apb1` and `Apb2` as well as in `const Rcc::PllFactors pllFactors{...}` and the
following lines. The `PllFactors` struct should be fairly self-explanatory.


### 2. Peripheral Mapping

As we can see in the graphic above, there are different clock ranges.
Each peripheral is connected to a clock domain.
Some peripherals have an upstream clock mux, this is currently ignored in
modm and the default setting for the clock mux is assumed.

The figure shows the block diagram of the controller, which can be found at
the beginning of the data sheet (not in the reference manual):

![STM32 Block Diagram example](https://user-images.githubusercontent.com/2820734/81448035-fffe0b00-917d-11ea-8a1f-428548851e63.png)

For each peripheral we create a `static constexpr uint32_t` member in
the `struct SystemClock` and assign the value of the clock domain to which
the peripheral is connected.
