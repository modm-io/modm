# External Interrupt/Event Controller (EXTI)

This driver provides an API for configuring all EXTI lines via register access.
Note that you need to pass a mask, which allows you to configure multiple EXTI
lines at once.

A typical use-case is to trigger on multiple GPIO inputs:

```cpp
// all trigger on both edges
Exti::setTriggers<GpioA0, GpioC4, GpioD6>(Exti::Trigger::BothEdges);
// Sets the specific GPIO port per line
Exti::setTriggerSources<GpioA0, GpioC4, GpioD6>();

// Enables EXTI4 and EXTI9_5 IRQ handlers with the same priority of 10!
Exti::enableVectors<GpioC4, GpioD6>(10);
// Sets a different priority for EXTI0
Exti::enableVector<GpioA0>(5);
// Enable all interrupts at the same time
Exti::enableInterrupts<GpioA0, GpioC4, GpioD6>();

// You must manually define the IRQ handlers but *only*
// if you disable the `with_handlers` option!
MODM_ISR(EXTI0) { Exti::acknowledgeFlags<GpioA0>(); }
MODM_ISR(EXTI4) { Exti::acknowledgeFlags<GpioC4>(); }
MODM_ISR(EXTI9_5) { Exti::acknowledgeFlags<GpioD6>(); }
```

Note that you can also use this to configure more than just GPIO trigger
sources, you can configure all other device specific EXTI lines as well.
However, you need to manually configure the NVIC vector and possibly also
configure the sending peripheral separately.

```cpp
// Configure the RTC peripheral to send the tamper event
// Line 21 is the RTC tamper alert on STM32F4x6
Exti::setTrigger(1ul << 21, Exti::Trigger::RisingEdge);
Exti::enableInterrupts(1ul << 21);
// Manually enable the RTC_Alarm IRQ
NVIC_SetPriority(RTC_Alarm_IRQn, 10);
NVIC_EnableIRQ(RTC_Alarm_IRQn);
// Different IRQ naming scheme, not EXTI_RTC!
MODM_ISR(RTC_Alarm) {}
```

You can also trigger events instead of interrupts to facilitate WFE.

```cpp
Exti::setTrigger(1ul << 21, Exti::Trigger::FallingEdge);
Exti::enableEvents(1ul << 21);
```
