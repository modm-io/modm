# Internal Flash Memory

Erases and programs the internal flash memory. The STM32 families use nine
different flash controllers, each with its own implementation:

| Controller | Families       | Erase unit                     | Program unit      |
|:-----------|:---------------|:-------------------------------|:------------------|
| F1         | F0, F1, F3     | 1 or 2 KiB pages               | 16-bit            |
| F4         | F2, F4, F7     | 16 KiB to 256 KiB sectors      | 8, 16 or 32-bit   |
| L0         | L0, L1         | 128 or 256 byte pages          | 32-bit            |
| L4         | C0, G0, G4, L4, U0, WB, WL | 2, 4 or 8 KiB pages | 64-bit          |
| U5         | L5, U3, U5     | 2, 4 or 8 KiB pages            | 64 or 128-bit     |
| H5         | H5             | 8 KiB sectors                  | 128-bit           |
| H7         | H7             | 8 or 128 KiB sectors           | 128 or 256-bit    |
| H7RS       | H7R, H7S       | 8 KiB sectors                  | 128-bit           |
| WB0        | WB0, WL3       | 2 KiB pages                    | 32-bit            |

All controllers share the same interface: `Flash::unlock()` before any
operation, `Flash::getPage(addr)` or `Flash::getSector(addr)` to find the
erase unit of an address, `Flash::erase(index)` and
`Flash::program(addr, Flash::MaxWordType)`. The functions return the error
flags of the controller, zero on success. Pages and sectors are numbered
continuously over all banks. All addresses are absolute, only
`Flash::getOffset(index)` returns the offset from `Flash::OriginAddr`.
`Flash::MinPageSize` and `Flash::MaxPageSize` are the smallest and largest
page or sector of all flash organizations of the device, independent of the
dual-bank option bits. If they are equal, all pages have the same size.

## Interrupts during flash operations

Erasing and programming stall every read of the same flash bank, including
the fetch of interrupt vectors and handlers, and a core stalled on such a fetch
takes no other interrupt either, not even one with a higher priority. Only the
register sequence of each operation therefore runs from RAM (`modm_ramcode`),
and interrupts are masked during operations on the bank of the program:

- By default all interrupts are masked, and the `SysTick_Handler`, which this
  module places in RAM, is called by the flash driver while waiting, so that
  `modm::Clock` keeps counting.
- On cores with BASEPRI (not the Cortex-M0(+)) and with the vector table in RAM
  (`modm:platform:cortex-m:vector_table_location = ram`, the default on devices
  with DTCM or executable CCM), only interrupts with priority 0 may run instead,
  which includes the SysTick.
- The half-page programming of the STM32L1 always masks all interrupts, since
  any instruction fetch from flash causes a bus error until it finishes.

All other interrupts are delayed until the operation finishes, which can take
seconds for a large sector. If an application needs an interrupt during flash
operations, it must use a core with BASEPRI, put the vector table into RAM,
place the handler and all code it calls into RAM with `modm_ramcode`, and set
its priority to 0 before the flash operation (and back afterwards). Whether an application can miss
interrupts while writing the internal flash is a decision only it can make.

Operations on the other bank of dual-bank devices do not mask interrupts.

On dual-bank devices, an operation on a bank that contains none of the program
image sleeps for the typical duration of the operation from the datasheet and
then polls until it finished, so other fibers run meanwhile and the second bank
can be used as runtime storage without stalling the application. Sleeping needs
the `modm:platform:clock` module, otherwise the fiber only polls.
`Flash::program(addr, data, size)` yields between units on all devices. Only one
fiber may use the flash at a time, and no fiber may read the bank that is being
erased or programmed.

`Flash::program(addr, data, size)` programs a buffer with the fastest mode of
the controller: half-page programming on the L0/L1 (16x/32x faster than single
words), 128 byte bursts on the U5 and U3 (2.5x/1.7x faster) and 32-bit words on
the F2/F4/F7. The fast programming of the L4 controller is not used, since it
requires a mass erase before.

Units whose data has only erased bytes (`0xff`, `0x00` on the L0 and L1) are
skipped by `Flash::program(addr, data, size)`: programming them does not change
the content, but the L4 and U5 controllers refuse to program such a unit again
(PROGERR), although it still reads as erased. This also applies to the single
word `Flash::program(addr, word)`, which programs whatever it is given.

`Flash::WriteSize` is the smallest unit that `program()` can write, which
must be erased before. Most controllers protect each write unit with an ECC
and do not allow writing it twice.

A reset or power loss during programming can leave a flash word with a double
ECC error, which raises an NMI (C0 excluded: L4 and U5 controller, H5) or a
bus error (H7, H7RS) on every read. `Flash::read(addr, buffer, size)` copies
the flash without faulting and returns non-zero if it found such a word: the
NMI vector is redirected during the copy, or bus errors are ignored with
`FAULTMASK` and `SCB_CCR_BFHFNMIGN` (which also blocks all interrupts during
the copy). With the vector table in flash, this module defines the
`NMI_Handler` on these devices (other NMIs go to the default handler), so the
application may only define its own with the vector table in RAM. Tested by resetting during `program()` on the G071, G474, L476,
L552, U385, U575, H563, H723 and H743. A half-programmed word does not always
cause a double ECC error though: some read back with wrong data and no error
or with a "corrected" single error, so storage on top must use checksums.

Erased flash reads as `0xff`, except on the L0 and L1, where it reads as `0x00`.

Devices with a dual-bank option byte (F42x/F43x 1 MiB, F76x/F77x, G0B1, G4,
L4, L4+, L5, U3, U5) select the geometry from the option byte at runtime.
Bank swapping is not supported: the bank swap option must be cleared.
On the STM32WB the second core must not access the flash during operations.
