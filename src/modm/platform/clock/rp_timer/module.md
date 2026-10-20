# System Clock via RP2040 TIMER

The RP2040 TIMER is a 64-bit counter incremented by the 1µs watchdog tick and
shared by both cores. It is used to implement the `modm::Clock::now()` and
`modm::PreciseClock::now()` interfaces defined in the `modm:architecture:clock`
module without any interrupt.

Compared to the `modm:platform:clock` SysTick implementation, the clocks are
correct on both cores, since the SysTick is core-local, and they keep counting
while interrupts are masked.

```cpp
modm::platform::TimerClock::initialize<SystemClock>();
```

The microsecond clock reads the lower 32-bit of the counter directly. The
millisecond clock divides the 64-bit counter by 1000 with a multiply-shift,
since the Cortex-M0+ has no 64-bit division and no 32x32=64-bit multiplication.
