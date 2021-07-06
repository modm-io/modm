# Blocking Delays

These functions allow you to spin for a short time using only the CPU.

In general it is recommended to use the `std::chrono` duration types to allow
the compiler to choose the optimal implementation for you:

```cpp
#include <chrono>

constexpr std::chrono::nanoseconds DELAY_TIME{100};
modm::delay(DELAY_TIME);

constexpr std::chrono::microseconds DELAY_TIME2{200};
modm::delay(DELAY_TIME2); // same signature, different implementation
```

In your code you can also use the `std::chrono` literals:

```cpp
using namespace std::chrono_literals;

modm::delay(100ns);
modm::delay(200us);
modm::delay(300ms);
modm::delay(400s);
```

In order to not require wild casting around of values, there are also three
overloads for (unsigned) integer values. These are particularly useful for when
you do not want to or cannot use chrono literals.

```cpp
modm::delay_ns(100);
modm::delay_us(200);
modm::delay_ms(300);
```

Note that these delay functions work at any CPU clock speed, even if changed
dynamically at runtime and are available very early in the startup process at
hardware-init time.


## Limitations

The main limitations are accuracy and length of delay. The only guarantee given
to you is to delay for _at least_ the specified time. Note that invocation of
interrupts during spinning may add delay too.

- `modm::delay(ns)` guarantees at most 1'000'000ns = 1ms delay.
- `modm::delay(us)` guarantees at most 1'000'000µs = 1s delay.
- `modm::delay(ms)` is limited to 49 days.

Please note that the delay these functions provide is defined as the time from
invocation to the time of execution return. Obviously no delay beyond that is
considered, which may require you to use shorter delays to compensate for the
overhead of your code:

```cpp
do // GpioA4 toggling takes longer than 500ns because:
{
    modm::delay_ns(500); // takes ~500ns
    GpioA4::toggle();    // takes a few cycles
} while(1);              // jump back to loop also takes a few cycles
```

You should always prefer Software Timers (see `modm:processing:timer`) over
these *blocking* delay functions. However, when `modm::Clock` is not set up yet,
or when you need very small delays (for example to bit-bang a protocol), you
need to use these delay functions.

For the technical details on the delay implementations you can [read "Accurate
Micro- and Nanosecond Delay in modm"](https://blog.salkinium.com/modm-delay).
