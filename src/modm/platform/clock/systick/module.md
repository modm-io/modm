# System Clock via Cortex-M SysTick

The SysTick timer is used to provide a time point at milli- and microsecond
resolution to implement the `modm::Clock::now()` and `modm::PreciseClock::now()`
interfaces defined in the `modm:architecture:clock` module.

For this, the timer is configured to run at 1/8th HCLK frequency
and the 24-bit counter is loaded for an interrupt every 250ms (on ARMv7-M) or
every 1ms (ARMv6-M). The interrupt increments the 32-bit counters by 250ms and
250'000us (or 1ms and 1000us) respectively.

Since this comes nowhere near the 1us resolution we want, the `SysTick->VAL`
counter is used to interpolate the time between interrupts with this algorithm:

```cpp
uint32_t milli_time{0};
uint32_t micro_time{0};
void SysTick_Handler()
{
	milli_time += 250;
	micro_time += 250'000;
}

modm::{Precise}Clock::now()
{
	float cycles_per_ms = (SystemClock::Frequency / 8) / 1e3;
	float cycles_per_us = (SystemClock::Frequency / 8) / 1e6;
	uint32_t val = SysTick->LOAD - SysTick->VAL; // SysTick->VAL counts down
	milliseconds = milli_time + val / cycles_per_ms;
	microseconds = micro_time + val / cycles_per_us;
}
```

The actual algorithm avoids the floating point division and is instead
approximated with a 32x32=64-bit multiplication and a bit shift without loosing
accuracy. This computation is very, very fast on ARMv7-M.

However, since ARMv6-M only has a 32x32=32-bit multiplication, the interrupt
runs at 1ms, thus eliminating the multiplication for the millisecond clock,
and allowing to fit into a 32-bit multiplication for the microsecond clock.
