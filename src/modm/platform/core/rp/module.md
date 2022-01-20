# RP2040 core module

This module specializes the generic `modm:platform:cortex-m` module with
RP2040-specific startup code and linkerscripts.

!!! note "Second-stage Bootloader"
	The external Flash memory requires additional setup, which is provided by
	the second-stage bootloader called boot2. You can select a pre-defined
	configuration via the `modm:platform:core:boot2` option.


## Startup

The `__modm_initialize_platform()` callback resets all hardware to a known
initial state.


## Linkerscript

The RP2040 currently only has one linkerscript template for internal SRAM and
external Flash. You can place static objects in sections via the
`modm_section` attribute:

```cpp
// .data sections get copied from flash to RAM during startup
modm_section(".data_core0")
uint64_t data = 0x1234567812345678ull;

// .bss sections are not stored in flash but get zeroed during startup
modm_section(".bss_core0")
uint8_t buffer[1024];

// .noinit sections are left uninitialized
modm_section(".noinit")
uint8_t display_buffer[480][320];
```


### Dual-Core Static RAM (SRAM)

The RP2040 memory map consists out of the main RAM (SRAM0-3 with striped bank
access) and the dedicated memories for each core (SRAM4-5).
Note that the stack size for both cores is the same and can be changed with the
`modm:platform:cortex-m:main_stack_size` option, however, the main stack for
CPU1 is only added if modm is build with the `modm:platform:multicore` module
to free up the unused memory.

```
            ┌────────────────────────┐◄ __core0_end
            │  .noinit_core0         │
            │  .bss_core0            │
            │  .data_core0           │
   CORE0    │  +MAIN_STACK_SIZE      │◄ __main_stack_top
0x2004 1000 ├────────────────────────┤◄ __core1_end, __core0_start
            │  .noinit_core1         │
            │  .bss_core1            │
            │  .data_core1           │
   CORE1    │ (+MAIN_STACK_SIZE)     │◄ __main1_stack_top (only with :platform:multicore)
0x2004 0000 ├────────────────────────┤◄ __ram_end, __core1_start
            │  +HEAP_RAM             │
            │  .noinit_ram           │
            │  .noinit               │
            │  .faststack            │
            │  .bss_ram              │
            │  .bss                  │
            │  .data_ram             │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
    RAM     │ (.vector_ram)          │◄ only if remapped into RAM
0x2000 0000 └────────────────────────┘◄ __ram_start

            ┌────────────────────────┐◄ __flash_end
            │        (unused)        │
            ├────────────────────────┤◄ __rom_end
            │  .table.heap           │
            │  .table.copy.extern    │
   tables   │  .table.zero.extern    │
            │  .table.copy.intern    │
            │  .table.zero.intern    │
            │                        │
            │  .data_core1           │
            │  .data_core0           │
    copy    │  .data_ram             │
    only    │  .data                 │
            │  .fastcode             │
            │  .fastdata             │
            │                        │
            │  .note.gnu.build-id    │
            │  .assertion            │
            │  .hardware_init        │
            │ (.eh_frame)            │
    read    │ (.ARM.exidx)           │  only with C++ exceptions enabled
    only    │ (.ARM.extab)           │
            │  .init_array           │
            │  .init                 │
            │  .rodata               │
            │  .text                 │
            │  .vector_rom           │
   FLASH    │  .boot2                │◄ see :platform:core:boot2 option
0x1000 0000 └────────────────────────┘◄ __rom_start, __flash_start
```
