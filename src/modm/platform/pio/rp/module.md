# Programmable IO (PIO)

This module provides compile-time PIO program assembly and runtime access to
RP2040/RP2350 PIO blocks and their state machines.

PIO lets you implement custom serial protocols, precise timing loops, and other
bit-banging tasks in dedicated hardware while the CPU does other work. Programs
are encoded at compile time into instruction arrays and loaded into PIO
instruction memory at runtime.


## Quick Start

```cpp
#include <modm/platform/pio/pio_asm.hpp>
#include <modm/platform/pio/pio_program.hpp>

using namespace modm::platform;
using namespace modm::platform::pio;

struct bitloop {};

static constexpr auto program = PIOProgram::begin()
    .label<bitloop>
        .instr(pio::Out.pins<1>)
        .instr(pio::Jmp.to<bitloop>)
    .end();

// Load into PIO0 and start state machine 0
const size_t offset = Pio0::addProgram(program);
auto config = pio::sm_config(offset, program);
Pio0::StateMachine<0>::init(config, offset);
Pio0::StateMachine<0>::setEnabled(true);
```

See the `rp_pico/pio_ws2812` example for a complete translation of a Pico SDK
`.pio` file including sideset, wrap, labels, and frequency configuration.


## Translating from `.pio` Assembly

The modm PIO assembler mirrors the Raspberry Pi Pico SDK `.pio` syntax, but
expresses it as a type-safe C++ builder API in `pio_asm.hpp`.

| `.pio` syntax | modm C++ equivalent |
|---|---|
| `.program name` | `PIOProgram::begin()` |
| `.side_set N` | `.sideset<N>()` |
| `.side_set N opt` | `.sideset_opt<N>()` |
| `.wrap_target` | `.wrapTarget<>()` |
| `.wrap` | `.wrap<>()` |
| `label:` | `.label<label>()` |
| `jmp target` | `pio::Jmp.to<target>()` |
| `jmp !x target` | `pio::Jmp.not_x.to<target>()` |
| `wait 0 gpio 3` | `pio::Wait<0>.gpio<3>()` |
| `in pins, 3` | `pio::In.pins<3>()` |
| `out pins, 3` | `pio::Out.pins<3>()` |
| `push` / `pull` | `pio::Push` / `pio::Pull` |
| `mov x, y` | `pio::Mov.x.y` |
| `irq set 3` | `pio::Irq.set<3>()` |
| `set pins, 3` | `pio::Set.pins<3>()` |
| `nop` | `pio::Nop` |
| `side 1 [5]` | `.side<1>.delay<5>()` on the instruction |

Instructions are added with `.instr(...)`. Chain builder calls from
`PIOProgram::begin()` and finish with `.end()`.

```cpp
// Original .pio:
// .side_set 1
// .wrap_target
// bitloop:
//     out x, 1       side 0 [T3 - 1]
//     jmp !x do_zero side 1 [T1 - 1]
// do_one:
//     jmp  bitloop   side 1 [T2 - 1]
// do_zero:
//     nop            side 0 [T2 - 1]
// .wrap

struct bitloop {};
struct do_one {};
struct do_zero {};

static constexpr auto program = PIOProgram::begin()
    .sideset<1>()
    .wrapTarget<>()
    .label<bitloop>
        .instr(pio::Out.x<1>              .side<0>.delay<T3 - 1>())
        .instr(pio::Jmp.not_x.to<do_zero> .side<1>.delay<T1 - 1>())
    .label<do_one>
        .instr(pio::Jmp.to<bitloop>       .side<1>.delay<T2 - 1>())
    .label<do_zero>
        .instr(pio::Nop                  .side<0>.delay<T2 - 1>())
    .wrap<>()
    .end();
```


## Labels

Labels are **empty struct types**, not strings. Define one struct per label and
use it as a template argument:

```cpp
struct my_label {};
// ...
.label<my_label>
    .instr(...)
```

Jump to a label with `.to<Label>()` on any `Jmp` variant:

```cpp
pio::Jmp.to<my_label>()
pio::Jmp.not_x.to<my_label>()
pio::Jmp.y_dec.to<my_label>()
```

!!! warning "Label placement matters"
    `.label<Name>()` records the **next** instruction index as the label
    address. Place it immediately before the instructions that belong to that
    label, just like in `.pio` assembly.

!!! warning "Each label name must be unique"
    Defining the same label twice triggers a compile-time error.

!!! warning "Forward references are fine, but labels must exist"
    Jumps may reference labels defined later in the program. All labels are
    resolved when `.end()` is called. A jump to an undefined label fails at
    compile time.

To start execution at a specific label rather than the program beginning, pass
the label offset to `StateMachine::init()`:

```cpp
SM::init(config, offset + program.getOffset<bitloop>());
```

Numeric jump targets (without labels) are also supported via `pio::Jmp<>` with an
address template parameter, but labels are strongly preferred for readable code.


## Side-Set and Delay

Configure side-set bits before adding instructions:

- `.sideset<N>()` — N side-set bits, always present on every instruction.
- `.sideset_opt<N>()` — optional side-set (up to 4 bits); instructions without
  `.side<>` must still be valid for this mode.

Attach side-set value and cycle delay to individual instructions:

```cpp
.instr(pio::Out.pins<1>.side<1>.delay<5>())
.instr(pio::Out.pins<1>.delay<3>())          // delay only
.instr(pio::Out.pins<1>.side<0>())           // side only
```

Delay and side-set values are checked at compile time against the limits
imposed by the chosen `.sideset<>` / `.sideset_opt<>` configuration.


## Wrap

Use `.wrapTarget<>()` and `.wrap<>()` to set the loop boundaries, equivalent to
`.wrap_target` and `.wrap` in `.pio` files. Wrap addresses are applied relative
to the program offset when the state machine is configured via `sm_config()`.


## Loading and Running Programs

1. Build the program with `PIOProgram::begin() ... .end()`.
2. Load it: `size_t offset = PioN::addProgram(program);`
3. Configure the state machine: `auto cfg = pio::sm_config(offset, program);`
4. Customize pins, shift, FIFO, and clock: chain methods on `cfg`.
5. Initialize and enable: `PioN::StateMachine<M>::init(cfg, start_offset);`

```cpp
auto config = pio::sm_config(offset, program)
    .template setSidesetPins<DataGpio>()
    .template setOutShift<false, true, 24>()
    .setFifoJoinTx()
    .template setFrequency<SystemClock, 2'000'000>();

Pio0::StateMachine<0>::init(config, offset);
Pio0::StateMachine<0>::setEnabled(true);
```

Connect GPIO pins to the PIO block with `PioN::connect<Signals...>()` or
`PioN::connectPin<Pin>()`, and set pin directions via
`StateMachine::setPinsDirection<Pin>(true)`.

The TX/RX FIFOs block by default (using fibers). 
