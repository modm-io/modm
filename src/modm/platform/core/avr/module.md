# AVR Core

Implements the interfaces defined in the `:architecture` modules. Adds the
`-mmcu={target}` compile flag and the `F_CPU={core frequency}` define.


## Fuses

You can define the fuse values in your source code via the [`FUSES` struct ELF
section][fuses] mechanism, they are automatically used by avrdude. For fuse
values see the [AVR Fuse Calculator][fusecalc].

```c
#include <avr/io.h>
FUSES =
{
    LFUSE_DEFAULT, // .low
    HFUSE_DEFAULT, // .high
    EFUSE_DEFAULT, // .extended
};
```


## Blocking Delay

The delay functions as defined by `modm:architecture:delay` and called with a
**constant value** are implemented directly via GCC's
`__builtin_avr_delay_cycles(ns / (1e9 / F_CPU))`. Constant delays are therefore
fairly accurate.

For delays with a dynamic time, the following limitations apply for nanoseconds:

- The input is limited to 26-bit nanoseconds, so ~67 milliseconds.
- The minimum delay is between 8-16 cycles, so even at the fastest clock rate of
  32MHz a delay below ~500ns is not possible. At 8MHz delays less than 2us are
  not possible.
- For longer delays, the 4-cycle `_delay_loop_2(loops)` is used and the number
  of loops should be calculated by dividing the input value by `ns_per_loop =
  4e9/F_CPU`. However, division is prohibitively slow and thus very coarsly
  approximated by shifting the input: `ns >> ceil(log_2(ns_per_loop))`. This is
  only accurate for `4e9/2^8` = 15.6MHz, 7.8MHz, 3.9MHz, etc. For all other
  clock speeds the error is bound by a factor of 2, so the delay is at most
  twice as long as expected.

For micro- and milliseconds delays with dynamic time:

- Microseconds delay is implemented fairly accurately in 1us steps.
- Millisecond delay is implemented fairly accurately in 1ms steps on 32-bits of
  input time.


[fuses]: https://www.nongnu.org/avr-libc/user-manual/group__avr__fuse.html
[fusecalc]: https://www.engbedded.com/fusecalc/
