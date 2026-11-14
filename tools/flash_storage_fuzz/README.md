# Power-loss fuzzer for flash storage libraries

Runs Zephyr ZMS (key-value) on an emulated flash in a host program, cuts the
power at random points and checks every byte of the stored data against a
reference model after each reboot. A simple reference store (`golden`) checks
the harness itself: it must survive every model.

```sh
OUT=build ./build.sh   # ZMS=, SUFFIX= select other sources, e.g. upstream
build/zms seeds=10 cap=1000 family=l4
build/zms seeds=10 cap=1000 cut=prefix erase=atomic prog=content when=ops
build/zms wear=20000 prog=content   # no power loss: wear and double writes
build/zms seed=7 unit=32 trace=1 ...   # one seed with every flash operation and reset
```

## What is tested

- 16 keys, values of 1-64 bytes, 1/8 of the values end with 0xFF bytes (like
  -1, NaN or structs with erased defaults). Per step: 60% write, 10% delete,
  30% only read a random key.
- After every reboot all keys are read: the key whose update was interrupted
  must have its old or its new value (or be deleted for an interrupted delete),
  every other key must be unchanged.
- A run stops at the **first** failure, later failures are only consequences.
  Each seed is a new run from erased flash; a seed that reaches `cap` resets
  without failure has survived.
- ZMS uses 4 sectors of 2 kB each.

## The power-loss model

`when`: when the power fails
- `ops`: after 0-199 flash operations, uniformly. Stresses rarely hit paths,
  like writing the close entry of a sector.
- `time`: after 0-2 s of flash activity, a write takes 80 µs per program unit
  and an erase 20 ms per sector. Realistic: most power losses hit an erase.

`cut`: what the interrupted program unit contains (units before it are written
completely, units after it are not touched)
- `unit`: nothing, the unit is still erased. The gentlest model.
- `prefix`: its first 0 to unit-1 bytes, like the `max_len` of the Zephyr flash simulator.
- `bits`: a random half of the bits that should be cleared. Closest to an
  interrupted program pulse on ECC flash, which programs a whole unit at once.
- `stm32`: measured on L4/L5/U3/U5/H5/H7 by resetting during `Flash::program()`:
  20% still erased, 50% complete, 30% damaged (wrong data
  with an ECC flag). A damaged unit cannot be programmed (`prog=content`). A unit
  that reads as erased never had an ECC flag on the hardware and could be
  programmed. Units of all ones data are assumed to behave the same (not
  measured, the hardware test used `0x5a`).

**The realistic models are the measured presets `family=l4|u5|h5|h7|f4|l0`**
(later options override them):

| family | unit | program | erase | interrupted unit: erased/complete/ECCC/ECCD | interrupted erase | reprogram fails | ECCD reads |
|---|---|---|---|---|---|---|---|
| l4 (L4/L5/G0/G4) | 8 | 92 µs | 22.1 ms | 19/56/0/25 | not seen, atomic | not erased, ECC, all ones before | raw |
| u5 (U5/U3) | 16 | 125 µs | 1.54 ms | 10/83/2/5 | bits drift to one, erased words from 12%, clean from 60% | like l4 | raw |
| h5 | 16 | 36 µs | 1.55 ms | 26/60/12/2 | not seen, atomic | not erased, ECC | raw |
| h7 | 32 | 86 µs | 0.93 s | 7/18/0/75 | zero in address order (0-15%), garbage (-36%), ECCD (-50%), clean | not erased, ECC | garbage |
| f4 (F2/F4/F7) | 1 | 4 µs | 0.19 s | 50/50/0/0 | not seen, atomic | never (NOR) | - |
| l0 (L0/L1) | 4 | 3.2 ms | 3.3 ms | 20/50/0/30 (unmeasured) | not seen, atomic | not erased, ECC | raw |

All presets cut by time (`when=time`), skip units of all ones like the driver
(`skipff=1`) and read ECCD units as zeros like the ports (`eccmap=1`). "Not seen"
means a system reset never interrupted the erase, real power loss is unmeasured.
The older `cut=stm32 erase=bits prog=stm32 when=time` is more pessimistic for
erases.

`erase`: what an interrupted erase leaves
- `atomic`: the sector is unchanged, the erase did not start.
- `bits`: random bits of the sector are set, a partially erased sector.

`prog`: when programming a unit fails with -EIO
- `nor`: never, bits are ANDed (NOR flash without ECC, STM32F2/F4/F7).
- `content`: if the unit does not read as erased, like the Zephyr flash simulator
  without `CONFIG_FLASH_SIMULATOR_DOUBLE_WRITES` (ECC flash with an erased check
  by reading, STM32 PROGERR).
- `stm32`: like `content`, and also if the unit was programmed with all ones
  before (completed, or interrupted after the first 20%): measured on L4 and U5,
  the word reads as erased but the next program fails with PROGERR. H5/H7
  accept it. `skipff=1` emulates a driver that does not program units of all
  ones, which avoids this.
- `touched`: if anything programmed the unit since the erase, even if it still
  reads as erased. Not realistic: on no measured STM32 did a word that reads
  as erased fail to program.

Reads outside the flash fail like the range check of a port and are counted.
Failed programs are counted as `double` (the library wrote a completely
written unit again: a bug on ECC flash even without power loss) or `residue`
(the unit was touched by an interrupted operation).

## Not modeled

- ECC errors of neighbouring words: the driver must only report the ECC errors
  of the words it read (prefetch reads the next word too).
- Interrupted erases by power loss (only by system reset) and the L0/L1 program.
- Bits that read correctly but are weakly programmed and flip later.
- Power loss during the reboot itself (mount and recovery run uninterrupted).

## Coverage guided with libFuzzer

`build.sh` also builds `zms-lf` and `golden-lf` with libFuzzer,
ASan and UBSan if a Homebrew LLVM is installed (Apple clang has no libFuzzer
runtime). The fuzzer input replaces the random numbers that choose the
structure: operations, IDs, values (up to 1800 bytes),
when the power fails and which kind of damage the interrupted unit gets. The
damaged bits themselves come from a separate generator, so that the fuzzer
cannot build CRC collisions byte by byte; a failure is only reported if it
repeats with at least 6 of 8 damage seeds. Every input is one run from erased
flash with up to 32 power losses and a final regular reboot.

```sh
mkdir corpus crashes
FUZZ_MODEL="cut=stm32 erase=bits prog=stm32 when=time unit=8 skipff=1" \
    build/zms-lf -max_total_time=1800 -max_len=4096 -artifact_prefix=crashes/ corpus
FUZZ_MODEL="unit=8 nopower=1" build/zms-lf ...             # algorithm only, no power loss
FUZZ_MODEL="... trace=1" build/zms-lf crashes/crash-...     # replay with every flash operation
build/zms-lf -minimize_crash=1 -max_total_time=60 crashes/crash-...
```

A write may fail with `-ENOSPC` only if the live values (plus 32 bytes each)
exceed `lib_capacity()`, one sector for ZMS.
