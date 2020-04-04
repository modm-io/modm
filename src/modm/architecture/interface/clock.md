# System Clock

To provide a system-wide time base this module provides two clocks:

- `modm::Clock`: a 32-bit unsigned millisecond clock running for approx. 49 days
  until wrap around, as an optimized replacement of `std::chrono::system_clock`.
- `modm::PreciseClock`: a 32-bit unsigned microsecond clock running for approx.
  71 minutes until wrap around, as an optimized replacement of
  `std::chrono::high_resolution_clock`.

These clocks are intended to measure time intervals:

```cpp
const auto start = modm::PreciseClock::now();
how_long_does_this_function_take();
const auto duration = modm::PreciseClock::now() - start;
MODM_LOG_DEBUG << "Function took " << duration << modm::endl;
```

For additional common timeout and periodic timer use-cases also check out the
`modm:processing:timer` module which uses these clocks.


## Limitations

The clocks intended to be very lightweight to use, however, a few interface
tradeoffs had to be made to achieve this goal.

1. The clocks are **not steady** with their 32-bit counters overflowing after a
   relatively short time. They must therefore only be used for measuring
   intervals, not absolute time.
2. The clocks have *unsigned* durations to make it easy to work with intervals
   since the addition/subtraction of intervals works just fine on clocks
   wrapping around, on the important condition that the interval is shorter than
   the clocks wrap around time.

!!! warning "Use a RTC-backed calendar for timekeeping!"
    These clocks are definitely not accurate over long periods of time, since
    they are based on the internal clock supplies, which may have significant
    drift and thus are no replacement for a Real-Time Clock!

Also check the `modm:platform:clock` module descriptions for additional
limitations on clock resolution and clock drift depending on target.


