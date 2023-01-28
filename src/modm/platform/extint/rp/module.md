# External Interrupt Handler

This driver provides an API for configuring all IRQ lines via register access.

```cpp
// Powers on the LED on the low->high transition, and off on high->low.
GpioInput0::setInput();
IntHandler::connect<GpioInput0>(Gpio::InputTrigger::BothEdges,
                                [](Gpio::InputTrigger_t triggers) {
                                    Led::set(!!(triggers & Gpio::InputTrigger::RisingEdge));
                                });

// Toggles LED each time gpio input is at the high level.
GpioInput1::setInput(Gpio::InputType::PullDown);
IntHandler::connect<GpioInput1>(Gpio::InputTrigger::HighLevel,
                                [](Gpio::InputTrigger_t) { Led::toggle(); });
```

## Multicore mode

Each core can register callbacks in the same IntHandler,
but for the different pins (current implementation's constraint).

Also, enable/disable and connect/disconnect calls
affect the NVIC of the executing core only.

## Callbacks

The callback is implemented using `modm::inplace_function`, therefore uses no
heap, but has a fixed storage size of `sizeof(void*)` by default.
You can increase this storage size by defining a new global storage size
`MODM_EXTINT_HANDLER_STORAGE=bytes` in your `project.xml`:

```xml
<library>
  <collectors>
    <collect name="modm:build:cppdefines">MODM_EXTINT_HANDLER_STORAGE=12</collect>
  </collectors>
</library>
```

## IRQ Types

You can explicitly enable or disable handling of the specific IRQ type in your `project.xml`:

```xml
<library>
  <options>
    <option name="modm:platform:extint:gpio">no</option>
    <option name="modm:platform:extint:qspi">yes</option>
  </options>
</library>
```