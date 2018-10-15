# Blocking Delays

These functions allow you to spin for a short time using only the CPU.

You should always prefer Software Timers (see `modm:processing:timer`) over these
*blocking* delay functions. However, when `modm::Clock` is not set up yet, or
when you need very small delays (for example to bit-bang a protocol), you need
to use these delay functions.

The only guarantee given to you is to delay for _at least_ the specified time.
Note that invocation of interrupts during spinning may add delay too.

!!!warning
   You must not rely on delay functions for ANY time-keeping!

Delay functions work at any CPU clock speed, even if changed dynamically and
are available very early in the startup process at hardware-init time.

!!!warning
   Correct behavior is not guaranteed for delays over 1000ns, us or ms!
   Make sure to use the largest time unit possible.
