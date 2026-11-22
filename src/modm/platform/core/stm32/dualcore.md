# STM32H7 Dual-Core

The STM32H745/755/747/757 contain a Cortex-M7 and a Cortex-M4 core with
separate flash banks, RAMs and interrupt controllers, but they share all
peripherals and the SRAM4. Each core runs its own program, so you need one
project per core, which differ only in the target:

```xml
<!-- m7/project.xml -->
<option name="modm:target">stm32h745zit6@m7</option>
<!-- m4/project.xml -->
<option name="modm:target">stm32h745zit6@m4</option>
```

The Cortex-M7 links into flash bank 1, the ITCM, DTCM, AXI SRAM, SRAM4 and
the backup SRAM, the Cortex-M4 into flash bank 2 and the D2 SRAMs. The Cortex-M7
does not cache the 64kB SRAM4, so it holds the memory shared by both cores.


## Startup

Both cores boot at the same time, but only the Cortex-M7 configures the power
supply and the clock tree. The Cortex-M4 therefore waits in `wait()` until the
Cortex-M7 calls `start()` after its initialization:

```cpp
// Cortex-M7
int main()
{
    Board::initialize();
    modm::platform::dualcore::start();
}
// Cortex-M4
int main()
{
    modm::platform::dualcore::wait(); // Board::initialize() calls this
}
```

The handshake uses the interrupt status of hardware semaphore 0, so it works
regardless of which core gets there first. `start()` also boots the Cortex-M4
if the `BCM4` option byte holds it.


## Shared Memory

Define the content of the shared memory as one type in a header that both
projects include. The Cortex-M7 places an object of it into SRAM4, where its
startup code initializes it before `main()`, and passes its address to the
Cortex-M4 with `start(object)`:

```cpp
struct Shared
{
    modm::atomic::Queue<Message, 16> toM4;
    modm::atomic::Queue<Message, 16> toM7;
    std::atomic<uint32_t> state;
    uint32_t counter;
    std::span<uint8_t> buffer;
};
// Cortex-M7
modm_section(".noinit_d3_sram") uint8_t buffer[32*1024];
modm_section(".data_d3_sram") constinit Shared shared{.buffer = buffer};
int main()
{
    Board::initialize();
    modm::platform::dualcore::start(shared);
}
// Cortex-M4
int main()
{
    Board::initialize(); // calls wait()
    Shared &shared = modm::platform::dualcore::shared<Shared>();
}
```

The address is passed through a mailbox in the first 8 bytes of SRAM4, so only
the Cortex-M7 decides the layout of SRAM4 and both projects build on their
own. `shared<T>()` asserts that both cores were built with the same type, so
that a stale program on the other core is noticed. The shared object may
reference other objects in SRAM4, like the buffer above, whose `std::span` also
passes its length. The Cortex-M7 places them into `.data_d3_sram`,
`.bss_d3_sram` or `.noinit_d3_sram`.

Global and static variables of code that both projects compile, for example
of a common module, exist once per core in its own RAM. Only what the shared
object references is shared between the cores.

`modm::atomic::Queue` exchanges messages between both cores, each queue with
exactly one producer and one consumer. A fiber can wait on it with
`modm::this_fiber::poll([&]{ return queue.isNotEmpty(); })`, which also works
without a fiber scheduler, so each core can use fibers or not.

Loads and stores of `std::atomic` are atomic between both cores, however,
read-modify-write operations like `fetch_add()` are not, since the bus has no
global exclusive monitor. Protect them with a hardware semaphore instead:

```cpp
{
    modm::platform::dualcore::Lock<1> lock; // semaphores 1-31
    shared.counter++;
}
```

The lock also disables interrupts on its core, but it does not protect against
other fibers on the same core, so do not yield while holding it.

!!! warning "Fiber primitives only work within one core"
    `modm::fiber::mutex`, semaphores, latches, barriers, shared mutexes,
    condition variables and stop tokens use atomic read-modify-write operations,
    so do not place them into the shared object. Use `modm::atomic::Queue` or
    `Lock<id>` between the cores instead.


## Object Lifetime

The Cortex-M7 owns the shared object: its startup code creates it and the
Cortex-M4 only ever receives a reference to it. This has some consequences:

- Prefer `constinit` for the shared object. A constructor runs before `main()`
  and before the clocks are configured, in an unspecified order relative to
  global objects in other source files.
- The startup code does not initialize `.noinit_d3_sram`, so use it for raw
  buffers only. Construct anything else there with placement new before calling
  `start(object)`.
- SRAM4 keeps its content across a reset, so the mailbox can still hold the
  object of the previous run. `shared<T>()` therefore asserts that `start()` or
  `wait()` has returned, so you cannot use it to initialize a global variable.
- The shared type must not be copyable, so that `auto shared = shared<T>()`
  with a missing `&` cannot silently create a private copy. Types containing a
  `std::atomic` or a `modm::atomic::Queue` are already not copyable, otherwise
  delete the copy constructor.
- The shared type must not have virtual functions, since its vtable only exists
  in the flash of the Cortex-M7. For the same reason, do not share function
  pointers, or pointers to the private RAM or the heap of one core.
- Each compiler only sees the accesses of its own core. Any member that the
  other core changes concurrently must be a `std::atomic` or be protected by a
  `Lock`, otherwise the compiler may remove or reorder the accesses.
- Destructors of the shared object never run on the Cortex-M4, and returning
  from `main()` on the Cortex-M7 resets the device.
- Formally, no object of type `T` ever exists in the program of the Cortex-M4,
  and `std::start_lifetime_as` cannot help since `std::atomic` is not an
  implicit-lifetime type. As with memory-mapped registers, this works since the
  address comes from a volatile load that the compiler cannot see through.


## Reset

A software reset from either core with `NVIC_SystemReset()`, an independent
watchdog and the NRST pin all reset the whole device including both cores. The
SRAMs keep their content. Each core reads the cause from its own view of
`RCC->RSR` and clears it with the `RMVF` bit without affecting the other core.

Only the window watchdogs reset just their own core by default: a restarting
Cortex-M7 would re-initialize the shared object while the Cortex-M4 uses it, and
a restarting Cortex-M4 would wait forever in `wait()`. Set `RCC_GCR_WW1RSC` and
`RCC_GCR_WW2RSC` before enabling WWDG1 or WWDG2, which selects a system reset
and is required by RM0399 9.7.36.


## Programming and Debugging

Program each project on its own, in any order:

```sh
(cd m4 && scons program)
(cd m7 && scons program)
```

Each call resets both cores, so the new program briefly runs with the old
program of the other core, which `shared<T>()` notices if the shared type
changed. `scons debug` attaches GDB to the core of its project.

To debug both cores at the same time, start one OpenOCD in the Cortex-M7
project, which serves the Cortex-M7 on port 3333 and the Cortex-M4 on port 3334,
and attach one GDB per core:

```sh
(cd m7 && scons openocd)
(cd m7 && scons debug-remote)
(cd m4 && scons debug-remote host=:3334)
```

On the NUCLEO-H745ZI-Q, a halt of one core also halts the other one, however,
each core only resumes on its own, so `continue` in both GDB sessions.


## Peripherals

Each program has its own copy of the HAL state and its own interrupt controller,
so every peripheral should belong to exactly one core, which calls its
`initialize()` function and handles its interrupts. The hardware registers are
shared and `modm::atomic::Lock` only protects against the same core, so anything
that both cores modify must be set up before `start()` or use a `Lock<id>`:

- **Clocks**: The Cortex-M7 configures the clock tree and the kernel clock
  sources before `start()`, the Cortex-M4 only stores the resulting frequencies.
  Do not change them afterwards.
- **RCC**: Each core enables peripherals in its own registers, and a peripheral
  stays clocked while at least one core has it enabled. It is only reset when
  neither core has enabled it yet.
- **GPIO**: Setting and resetting pins is atomic. All pins of a port share the
  mode and alternate function registers, so configure them on the Cortex-M7
  before `start()`. `toggle()` reads the output first, so only one core should
  drive a pin.
- **EXTI**: Each core has its own interrupt mask, event mask and pending flags.
  The trigger edges and the port selection in `SYSCFG` are shared.
- **DMA**: Assign each controller to one core. DMA1, DMA2 and DMA2D can access
  the D2 SRAMs of the Cortex-M4 through their alias at 0x1000'0000. The BDMA
  cannot access the D2 SRAMs, so place its buffers into SRAM4.
- **FDCAN**: The instances share the message RAM, which is split equally
  between them, so that each project can use its own instances.
- **ADC**: ADC1 and ADC2 share their common registers, so assign both to the
  same core.
- **USB**: Enabling the USB regulator modifies the shared `PWR_CR3` register, so
  do not initialize USB while the other core changes the power configuration.
- **Internal flash**: Use the flash driver only on the Cortex-M7, which
  invalidates its D-Cache after erasing and programming.
- **Bootloader**: `modm:platform:sysboot` is only available on the Cortex-M7.
- **Watchdog**: `Iwdg` uses IWDG1 on the Cortex-M7 and IWDG2 on the Cortex-M4,
  so each core feeds its own watchdog. A timeout of either resets both cores.
