# ARM Cortex-M Core

This module generates the startup code, vector table, linkerscript as well as
initialize the heap, deal with assertions, provide blocking delay functions,
atomic and unaligned access and the GNU build ID.

Since this is only initializes the generic ARM Cortex-M parts, it delegates
device-specific initialization to the `modm:platform:core` module. Please depend
on that module directly instead of this one.


## Startup

After reset, the ARM Cortex-M hardware jumps to the `Reset_Handler()`, which is
implemented as follows:

1. The main stack pointer (MSP) is initialized by software.
2. Call `__modm_initialize_platform()` to initialize the device hardware.
3. Call `modm_initialize_platform()` to initialize the custom device hardware.
4. Copy data to internal RAM.
5. Zero sections in internal RAM.
6. Initialize ARM Cortex-M core: enable FPU and relocate vector table.
7. Execute shared hardware initialization functions.
8. Copy data to *external* RAM.
9. Zero sections in *external* RAM.
10. Initialize heap via `__modm_initialize_memory()` (implemented by the
    `modm:platform:heap` module).
11. Call static constructors.
12. Call `main()` application entry point.
13. If `main()` returns, assert on `main.exit` (only in debug profile).
14. Reboot if assertion returns.


### Device Initialization

The `__modm_initialize_platform()` function is called *directly* after reset,
and its purpose is to initialize the device specific hardware, such as enable
internal memories or disable the hardware watchdog timer. You can provide
additional application-specific initialization by overwriting the weakly linked
`modm_initialize_platform()` function:

```c
extern "C" void modm_initialize_platform()
{
    // Configure power settings before accessing SRAM
}
```

It's important to understand that because the `.data` section has not yet been
copied and the `.bss` section has not yet been zeroed, **there exists no valid C
environment yet in this function context**! This means you cannot use any global
variables, not even "local" static  ones defined in your function, and depending
on your hardware you may not even access read-only data (`const` variables,
global OR local). In addition, if your linkerscript places the main stack
pointer into a memory that is disabled on reset, you cannot even access the
stack until you've enabled its backing memory. The `Reset_Handler` therefore
calls this function in Assembly without accessing the stack.

It is strongly recommended to only read/write registers in this function, and
perhaps even write this function in Assembly if deemed necessary.


### Additional Initialization

A few modules need to initialize additional hardware during booting. For
example: your device has external memories connected that you want to use for
the heap. You can create a function that configures the peripherals for these
external memories and place *a pointer to this function* into a special linker
section and the startup script will then call this function before heap
initialization.

Since the hardware init functions are called *after* internal data
initialization, you have a valid C environment and thus can access the device
normally, but since the calls happen *before* external data and heap
initialization **you cannot use the heap in these functions**!

You can give a relative global order to your init functions. Ordered init
functions are called first, then unordered init functions are called in any
order. Please note that order numbers 0 - 999 are reserved for use by modm or
other libraries!

!!! bug "Unique init function names"
    Init function names need to be globally unique for linking. Unfortunately
    there is no simple way of stringifying C++ functions, so you have to provide
    a name manually for now.

```cpp
void init_external_sdram()
{
	// configure the hardware here
}
// Startup script calls this function in any order, *after* prioritized functions!
MODM_HARDWARE_INIT(init_external_sdram);
// If you need to pass a C++ function, you need to declare
MODM_HARDWARE_INIT_NAME(init_function_name, namespace::init_function);

// If you need to initialize in a certain order use numbers >= 1000
MODM_HARDWARE_INIT_ORDER(init_before_sdram1, 1000);
// called after init_before_sdram1, since it has a higher order number
MODM_HARDWARE_INIT_NAME_ORDER(init_before_sdram2, namespace::function, 1001);
```


## Interrupt Vector Table

The Cortex-M vector table (VTOR) is target-specific and generated using data
from modm-devices. The main stack pointer is allocated according to the
linkerscript and the `Reset_Handler` is defined by the startup script.

All handlers are weakly aliased to `Undefined_Handler`, which is called if an
IRQ is enabled, but no handler is defined for it. This default handler
determines the currectly active IRQ, sets its priority to the lowest level, and
disables the IRQ from firing again and then asserts on `nvic.undef`
with the (signed) IRQ number as context.

The lowering of the priority is necessary, since the assertion handlers (see
`modm:architecture:assert`) are called from within this active IRQ and its
priority should not prevent logging functionality (which might require a UART
interrupt to flush data out) from working correctly.


## Linkerscript

This module provides building blocks for GNU ld linkerscripts in the form of
Jinja macros that the `modm:platform:core` module *assembles* into a
linkerscript, depending on the memory architecture of the target chosen.

The following macros are available:

- `copyright()`: Copyright notice.

- `prefix()`: Contains `MEMORY` sections, output format and entry symbol and
  stack size definitions.

- `section_vector_rom(memory)`: places the read-only vector table into ROM
  `memory`.

- `section_vector_ram(memory)`: places the volatile vector table into RAM
  `memory`. You must satisfy alignment requirements externally.

- `section(memory, name, sections=[])`: place section `.{name}` into `memory`
  and include `sections` into the `.{name}` section.

- `section_stack(memory, start=None)`: place the main stack into `memory` after
  moving the location counter to `start`.

- `section_heap(memory, name, placement=None, sections=[])`: Add the noload
  `sections` to `memory` and fill up remaining space in `memory` with heap
  section `.{name}`. Argument `placement` can be used to place the section into
  a larger continuous section of which `memory` is just a subsection. The
  `__{name}_end` will be the maximum of the location counter and the `memory`
  section end address, so that previous sections will push this section back.

- `all_heap_sections(table_copy, table_heap)`: places the heap sections as
  described by `cont_ram_regions` of the `linkerscript` query.

- `section_rom(memory)`: place all read-only sections (`.text`, `.rodata` etc)
  into `memory`.

- `section_ram(memory, rom, sections=[])`: place all volatile sections (`.data`,
  `.bss` etc) into `memory` and load from `rom`. Adds `sections` into the
  `.data` section.

- `section_tables(memory, copy, zero, heap)`: place the zero, copy and heap
  table into `memory`.

- `section_rom_start(memory)`: place at ROM start.

- `section_rom_end(memory)`: place at ROM end.

- `section_debug()`: place debug sections at the very end.

Please consult the `modm:platform:core` documentation for the target-specific
arrangement of these section macros and for potential limitations that the
target's memory architecture poses.


### Section `.fastdata`

The `.fastdata` section is placed into a device specific data cache or into the
fastest RAM. Please note that the `.fastdata` section may be placed into RAM
that is only accessable to the Cortex-M core (via the Data-Bus), which can cause
issues with DMA access. However, the `.fastdata` section is not required to be
DMA-able and in such a case the developer needs to place the data into the
generic `.data` section or choose a device with a DMA-able fast RAM.


### Section `.fastcode`

The `.fastcode` section is placed into a device specific instruction cache (via
I-Code bus) or into the fastest executable RAM (via S-Bus).

From the Cortex-M3 Technical Reference Manual:

> 14.5 System Interface:
>
> The system interface is a 32-bit AHB-Lite bus. Instruction and vector fetches,
> and data and debug accesses to the System memory space, 0x20000000 - 0xDFFFFFFF,
> 0xE0100000 - 0xFFFFFFFF, are performed over this bus.
>
> 14.5.6 Pipelined instruction fetches:
>
> To provide a clean timing interface on the System bus, instruction and vector
> fetch requests to this bus are registered. This results in an additional cycle
> of latency because instructions fetched from the System bus take two cycles.
> This also means that back-to-back instruction fetches from the System bus are
> not possible.
>
> Note:
>   Instruction fetch requests to the ICode bus are not registered.
>   Performance critical code must run from the ICode interface.


### Adding Sections

The default linkerscripts only describe the internal memory, however, they can
be extended for external memories using the `linkerscript.*` collectors of this
module. For example, to add an external 16MB SDRAM to your device and place a
static data section there that is copied from flash and use the remainder for
heap access, these steps need to be performed:

Add the external SDRAM to the linkerscript's `MEMORY` statements in the
`project.xml` configuration:

```xml
<library>
  <collectors>
    <collect name="modm:platform:cortex-m:linkerscript.memory">
       SDRAM (rwx) : ORIGIN = 0xC0000000, LENGTH = 16M
    </collect>
  <collectors>
</library>
```

You can also declare this as Python code in a lbuild `module.lb` file (useful
for board support packages modules, see `modm:board`):

```python
env.collect(":platform:cortex-m:linkerscript.memory",
            "SDRAM (rwx) : ORIGIN = 0xC0000000, LENGTH = 16M")
```

Add a partition of the new memory to the linkerscripts `SECTION` statements.
Since collectors order is only preserved locally, make sure to add the sections
that depend on this order in *one value*. Here the previous value of the SDRAM
location counter is required to "fill up" the remaining memory with the external
heap section:

```python
linkerscript_sections = """
.data_sdram :
{
    __data_sdram_load = LOADADDR(.data_sdram);
    __data_sdram_start = .;

    *(.data_sdram)

    . = ALIGN(4);
    __data_sdram_end = .;
} >SDRAM AT >FLASH

.heap_sdram (NOLOAD) :
{
    __heap_sdram_start = .;
    . = ORIGIN(SDRAM) + LENGTH(SDRAM);
    __heap_sdram_end = .;
} >SDRAM
"""
env.collect(":platform:cortex-m:linkerscript.sections", linkerscript_sections)
```

Next, add the sections that need to be copied from ROM to RAM, here the contents
of the `.data_sdram` section is stored in the internal `FLASH` memory and needs
to be copied into SDRAM during the startup:

```python
linkerscript_copy = """
LONG(__data_sdram_load)
LONG(__data_sdram_start)
LONG(__data_sdram_end)
"""
env.collect(":platform:cortex-m:linkerscript.table_extern.copy", linkerscript_copy)
```

And finally, to register the remaining memory in SDRAM with the allocator, add
the memory range to the heap table. Remember to use the correct memory traits
for this memory, see `modm:architecture:memory` for the trait definitions:

```python
linkerscript_heap = """
LONG(0x801f)
LONG(__heap_sdram_start)
LONG(__heap_sdram_end)
"""
env.collect(":platform:cortex-m:linkerscript.table_extern.heap", linkerscript_heap)
```

!!! info "Linkerscript collectors are plain text"
    The collectors here only strip the leading/trailing whitespace and newlines
    and paste the result *as is* into the linkerscripts. No input validation is
    performed, so if you receive linker errors with your additions, please check
    the GNU LD documentation first.


## Blocking Delay

The delay functions as defined by `modm:architecture:delay` are implemented via
software loop (ARMv6-M devices) or hardware cycle counter (via `DWT->CYCCNT` on
ARMv7-M device) and have the following limitations expressed in cycles, which
depends on the configured CPU frequency:

- nanosecond delay is implemented as a tight loop with a minimum delay of <20
  cycles, a resolution of 1-4 cycles and a maximum delay of 32-bit cycles.
- microsecond delay has a maximum delay of 32-bit cycles.


## Compiler Options

This module adds these architecture specific [compiler options][options]:

- `-mcpu=cortex-m{type}`: the target to compile for.
- `-mthumb`: only Thumb2 instruction set is supported.
- `-mfloat-abi={soft, softfp, hard}`: the FPU ABI: `hard` is fastest.
- `-mfpu=fpv{4, 5}-{sp}-d16`: single or double precision FPU.
- `-fsingle-precision-constant`: if SP-FPU, treat all FP constants as SP.
- `-Wdouble-promotion`: if SP-FPU, warn if FPs are promoted to doubles.

In addition, these linker options are added:

- `-nostartfiles`: modm implements its own startup script.
- `-wrap,_{calloc, malloc, realloc, free}_r`: reimplemented Newlib with our own allocator.

[options]: https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
