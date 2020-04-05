# AVR Core

Implements the interfaces defined in the `:architecture` modules. Adds the
`-mmcu={target}` compile flag and the `F_CPU={core frequency}` define.


## Blocking Delay

The delay functions as defined by `modm:architecture:delay` and called with a
**constant value** are implemented directly via GCC's
`__builtin_avr_delay_cycles(ns / (1e9 / F_CPU))`. Constant delays are therefore
fairly accurate.

For delays with a dynamic time, the following limitations apply for nanoseconds:

- The input is limited to 16-bit nanoseconds, so ~65 microseconds.
- The minimum delay is between 5-10 cycles, so even at the fastest clock rate of
  32MHz a delay below ~250ns is not possible. At 8MHz delays less than 1us are
  not possible.
- For longer delays, the 3-cycle `_delay_loop_1(loops)` is used and the number
  of loops should be calculated by dividing the input value by `ns_per_loop =
  3e9/F_CPU`. However, division is prohibitively slow and thus very coarsly
  approximated by shifting the input: `ns >> ceil(log_2(ns_per_loop))`. This is
  only accurate for `3e9/2^7` = 23.4MHz, 11.7MHz, 5.8MHz, etc. For all other
  clock speeds the error is bound by a factor of 2, so the delay is at most
  twice as long as expected.

For micro- and milliseconds delays with dynamic time:

- Microseconds delay is implemented fairly accurately in 1us steps with a
  maximum time delay of 65ms for clocks larger than 6MHz, or .
- Millisecond delay is implemented fairly accurately in 1ms steps on 32-bits of
  input time.
