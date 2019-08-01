# STM32F469IDISCOVERY

Discovery kit with STM32F469NI MCU.

This BSPs `Board::initialize()` sets the board up with these settings:

- 180MHz CPU clock frequency in overdrive mode.
- 90MHz APB2 and 45MHz APB1 clock frequency.
- MODM_LOGGER initialized at 115.2kBaud outputting on STLink.
- 16MB SDRAM initialized for heap.
- Disables the display by default.

You can enable and use the on-board display in two ways:

1. Use the `modm::GraphicDisplay` returned by `Board::getDisplay()`, which
   initializes the display on first call and allocates the framebuffer in the
   external SDRAM. This is the simplest option.
2. Call `Board::initializeDisplay()` and then provide your own display buffer
   with `Board::setDisplayBuffer(void*)`.

!!! warning "Caveats"
    No double-buffering is implemented, so you may notice some tearing if you
    update the display fast enough.

Access to the capacitive touchscreen is provided in the `Board::ft6` namespace.
Call `Board::initializeTouchscreen()` to setup the peripherals.

[Product Link](https://www.st.com/en/evaluation-tools/32f469idiscovery.html)
