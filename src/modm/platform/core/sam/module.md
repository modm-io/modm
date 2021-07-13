# SAM core module

This module specializes the generic `modm:platform:cortex-m` module with
SAM-specific startup code and linkerscripts.


## Startup

The `__modm_initialize_platform()` callback is implemented here to change the
default QOS values to have the best performance and correct USB behaviour.


## Linkerscript

Currently only one basic linkerscript is supported.

```
            ┌────────────────────────┐◄ __ram_end
            │  +HEAP_RAM             │
            │  .noinit_ram           │
            │  .noinit               │
            │  .bss_ram              │
            │  .bss                  │
            │  .data_ram             │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
            │  +PROCESS_STACK_SIZE   │◄ __process_stack_top
    RAM     │  +MAIN_STACK_SIZE      │◄ __main_stack_top
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
            │  .data_ram             │
    copy    │  .data                 │
    only    │  .fastcode             │
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
   FLASH    │  .vector_rom           │
0x0800 0000 └────────────────────────┘◄ __rom_start, __flash_start
```

