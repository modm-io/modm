# STM32 core module

This module specializes the generic `modm:platform:cortex-m` module with
STM32-specific startup code and linkerscripts.


## Startup

The `__modm_initialize_platform()` callback enables the clock to `SYSCFG` or
`AFIO` and the clock to all internal SRAMs.


## Linkerscript

For each target a specific linkerscript is generated out of a template, which
move some sections into special RAMs depending on their purpose. You can place
static objects in sections via the `modm_section` attribute:

```cpp
// .data sections get copied from flash to RAM during startup
modm_section(".data_sram1")
uint64_t data = 0x1234567812345678ull;

// .bss sections are not stored in flash but get zeroed during startup
modm_section(".bss_sram2")
uint8_t buffer[1024];

// .noinit sections are left uninitialized
modm_section(".noinit_sram3")
uint8_t display_buffer[480][320];
```

Note that the absolute addresses shown here are only for reference and usually
differ on a specific device.


### Static RAM (SRAM)

The most common memory map consists out of the internal Flash and one or more
continuous SRAMs, which are grouped together in one large a `CONT_SRAM1` memory.
This allows SRAM1 to overflow into SRAM2 and SRAM3 by pushing all sections in
front of it further up and collapsing the heap sections. In that case there is
no guarantee that the SRAM specific sections (`.data_sramN` etc) will remain
within their original SRAM section, but you can place large objects into higher
SRAMs explicitly to free up the space in the lower sections.

```
            ┌────────────────────────┐◄ __sram3_end
            │ (+HEAP_SRAM3)          │
            │ (.noinit_sram3)        │
            │ (.bss_sram3)           │  only if SRAM3 exists
   SRAM3    │ (.data_sram3)          │
0x2003 0000 ├────────────────────────┤◄ __sram2_end, __sram3_start
            │ (+HEAP_SRAM2)          │
            │ (.noinit_sram2)        │
            │ (.bss_sram2)           │  only if SRAM2 exists
   SRAM2    │ (.data_sram2)          │
0x2002 0000 ├────────────────────────┤◄ __sram1_end, __sram2_start
            │  +HEAP_SRAM1           │
            │  .noinit_sram1         │
            │  .noinit               │
            │  .bss_sram1            │
            │  .bss                  │
            │  .data_sram1           │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
            │ (.vector_ram)          │◄ only if remapped into RAM
            │  +PROCESS_STACK_SIZE   │◄ __process_stack_top
   SRAM1    │  +MAIN_STACK_SIZE      │◄ __main_stack_top
0x2000 0000 └────────────────────────┘◄ __sram1_start

            ┌────────────────────────┐◄ __flash_end
            │        (unused)        │
            ├────────────────────────┤◄ __rom_end
            │  .table.heap           │
            │  .table.copy.extern    │
   tables   │  .table.zero.extern    │
            │  .table.copy.intern    │
            │  .table.zero.intern    │
            │                        │
            │ (.data_sram3)          │◄ only if SRAM3 exists
            │ (.data_sram2)          │◄ only if SRAM2 exists
    copy    │  .data_sram1           │
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
   FLASH    │  .vector_rom           │
0x0800 0000 └────────────────────────┘◄ __rom_start, __flash_start
```


### Data Core-Coupled RAM (DCCM)

Some STM32F4 have a battery-backed backup SRAM and a single-cycle CCM that is
only accessible to the core via the D-Code bus, thus the CCM is not DMA-able.

```
            ┌────────────────────────┐◄ __backup_end
            │  +HEAP_BACKUP          │
            │  .noinit_backup        │
            │  .bss_backup           │
   BACKUP   │  .data_backup          │
0x4002 4000 └────────────────────────┘◄ __backup_start

            ┌────────────────────────┐◄ __sram3_end
            │ (+HEAP_SRAM3)          │
            │ (.noinit_sram3)        │
            │ (.bss_sram3)           │  only if SRAM3 exists
   SRAM3    │ (.data_sram3)          │
0x2003 0000 ├────────────────────────┤◄ __sram2_end, __sram3_start
            │ (+HEAP_SRAM2)          │
            │ (.noinit_sram2)        │
            │ (.bss_sram2)           │  only if SRAM2 exists
   SRAM2    │ (.data_sram2)          │
0x2002 0000 ├────────────────────────┤◄ __sram1_end, __sram2_start
            │  +HEAP_SRAM1           │
            │  .noinit_sram1         │
            │  .noinit               │
            │  .bss_sram1            │
            │  .bss                  │
            │  .data_sram1           │
            │  .data                 │
            │  .fastcode             │
   SRAM1    │ (.vector_ram)          │◄ only if remapped into RAM
0x2000 0000 └────────────────────────┘◄ __sram1_start

            ┌────────────────────────┐◄ __ccm_end
            │  +HEAP_CCM             │
   D-Code   │  .noinit_ccm           │
    only    │  .bss_ccm              │
   access   │  .data_ccm             │
            │  .fastdata             │
            │  +PROCESS_STACK_SIZE   │◄ __process_stack_top
    CCM     │  +MAIN_STACK_SIZE      │◄ __main_stack_top
0x1000 0000 └────────────────────────┘◄ __ccm_start

            ┌────────────────────────┐◄ __flash_end
            │        (unused)        │
            ├────────────────────────┤◄ __rom_end
            │  .table.heap           │
            │  .table.copy.extern    │
   tables   │  .table.zero.extern    │
            │  .table.copy.intern    │
            │  .table.zero.intern    │
            │                        │
            │ (.data_sram3)          │◄ only if SRAM3 exists
            │ (.data_sram2)          │◄ only if SRAM2 exists
            │  .data_sram1           │
    copy    │  .data_ccm             │
    only    │  .data_backup          │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
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


### Instruction Core-Coupled RAM (ICCM)

Some STM32F3, STM32L4 and STM32G4 devices have a single-cycle CCM that is
accessible to the core via the I-Code and D-Code interface. Note that the CCM is
not DMA-able.

```
            ┌────────────────────────┐◄ __sram3_end
            │ (+HEAP_SRAM3)          │
            │ (.noinit_sram3)        │
            │ (.bss_sram3)           │  only if SRAM3 exists
   SRAM3    │ (.data_sram3)          │
0x2003 0000 ├────────────────────────┤◄ __sram2_end, __sram3_start
            │ (+HEAP_SRAM2)          │
            │ (.noinit_sram2)        │
            │ (.bss_sram2)           │  only if SRAM2 exists
   SRAM2    │ (.data_sram2)          │
0x2002 0000 ├────────────────────────┤◄ __sram1_end, __sram2_start
            │  +HEAP_SRAM1           │
            │  .noinit_sram1         │
            │  .noinit               │
            │  .bss_sram1            │
            │  .bss                  │
            │  .data_sram1           │
            │  .data                 │
            │  +PROCESS_STACK_SIZE   │◄ __process_stack_top
   SRAM1    │  +MAIN_STACK_SIZE      │◄ __main_stack_top
0x2000 0000 └────────────────────────┘◄ __sram1_start

            ┌────────────────────────┐◄ __ccm_end
            │  +HEAP_CCM             │
   D-Code   │  .noinit_ccm           │
   I-Code   │  .bss_ccm              │
    only    │  .data_ccm             │
   access   │  .fastdata             │
            │  .fastcode             │
    CCM     │  .vector_ram           │
0x1000 0000 └────────────────────────┘◄ __ccm_start

            ┌────────────────────────┐◄ __flash_end
            │        (unused)        │
            ├────────────────────────┤◄ __rom_end
            │  .table.heap           │
            │  .table.copy.extern    │
   tables   │  .table.zero.extern    │
            │  .table.copy.intern    │
            │  .table.zero.intern    │
            │                        │
            │ (.data_sram3)          │◄ only if SRAM3 exists
            │ (.data_sram2)          │◄ only if SRAM2 exists
            │  .data_sram1           │
    copy    │  .data_ccm             │
    only    │  .data_backup          │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
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


### Tightly-Coupled RAM (TCM)

The STM32F7 devices include an Instruction TCM (ITCM) and a data TCM (DTCM).
Note that the TCMs are DMA-able, but only the MDMA. Note that DTCM but will
overflow into the SRAM1/2 sections.

```
            ┌────────────────────────┐◄ __backup_end
            │  +HEAP_BACKUP          │
            │  .noinit_backup        │
            │  .bss_backup           │
   BACKUP   │  .data_backup          │
0x4002 4000 └────────────────────────┘◄ __backup_start

            ┌────────────────────────┐◄ __sram2_end
            │  +HEAP_SRAM2           │
            │  .noinit_sram2         │
            │  .bss_sram2            │
   SRAM2    │  .data_sram2           │
0x2002 0000 ├────────────────────────┤◄ __sram1_end, __sram2_start
            │  +HEAP_SRAM1           │
            │  .noinit_sram1         │
            │  .bss_sram1            │
   SRAM1    │  .data_sram1           │
0x2001 0000 ├────────────────────────┤◄ __dtcm_end, __sram1_start
            │  +HEAP_DTCM            │
            │  .noinit_dtcm          │
            │  .noinit               │
   D-Code   │  .bss_dtcm             │
    only    │  .bss                  │
   access   │  .data_dtcm            │
            │  .data                 │
            │  .fastdata             │
            │  +PROCESS_STACK_SIZE   │◄ __process_stack_top
    DTCM    │  +MAIN_STACK_SIZE      │◄ __main_stack_top
0x2000 0000 └────────────────────────┘◄ __dtcm_start

            ┌────────────────────────┐◄ __flash_end
            │        (unused)        │
            ├────────────────────────┤◄ __rom_end
            │  .table.heap           │
            │  .table.copy.extern    │
   tables   │  .table.zero.extern    │
            │  .table.copy.intern    │
            │  .table.zero.intern    │
            │                        │
            │  .data_sram2           │
            │  .data_sram1           │
            │  .data_dtcm            │
    copy    │  .data_itcm            │
    only    │  .data_backup          │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
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

            ┌────────────────────────┐◄ __itcm_end
            │  +HEAP_ITCM            │
            │  .noinit_itcm          │
            │  .bss_itcm             │
            │  .data_itcm            │
            │  .fastcode             │
    ITCM    │  .vector_ram           │
0x0000 0000 └────────────────────────┘◄ __itcm_start
```

The STM32H7 memory map is more complex with multiple SRAM regions in seperate
power domains D1, D2 and D3. Note that the main `.data` and `.bss` sections are
placed into the 128kB DTCM, but cannot overflow into D1_SRAM section.

```
            ┌────────────────────────┐◄ __backup_end
            │  +HEAP_BACKUP          │
            │  .noinit_backup        │
            │  .bss_backup           │
   BACKUP   │  .data_backup          │
0x3880 0000 └────────────────────────┘◄ __backup_start

            ┌────────────────────────┐◄ __d3_sram_end
            │  +HEAP_D3_SRAM         │
            │  .noinit_d3_sram       │
            │  .bss_d3_sram          │
  D3_SRAM   │  .data_d3_sram         │
0x3800 0000 └────────────────────────┘◄ __d3_sram_start

            ┌────────────────────────┐◄ __d2_sram3_end
            │ (+HEAP_D2_SRAM3)       │
            │ (.noinit_d2_sram3)     │
            │ (.bss_d2_sram3)        │  only if D2_SRAM3 exists
  D2_SRAM3  │ (.data_d2_sram3)       │
0x3004 0000 ├────────────────────────┤◄ __d2_sram2_end, __d2_sram3_start
            │  +HEAP_D2_SRAM2        │
            │  .noinit_d2_sram2      │
            │  .bss_d2_sram2         │
  D2_SRAM2  │  .data_d2_sram2        │
0x3002 0000 ├────────────────────────┤◄ __d2_sram1_end, __d2_sram2_start
            │  +HEAP_D2_SRAM1        │
            │  .noinit_d2_sram1      │
            │  .bss_d2_sram1         │
  D2_SRAM1  │  .data_d2_sram1        │
0x3000 0000 └────────────────────────┘◄ __d2_sram1_start

            ┌────────────────────────┐◄ __d1_sram3_end
            │ (+HEAP_D1_SRAM3)       │
            │ (.noinit_d1_sram3)     │
            │ (.bss_d1_sram3)        │  only if D1_SRAM3 exists
  D1_SRAM3  │ (.data_d1_sram3)       │
0x240A 0000 ├────────────────────────┤◄ __d1_sram2_end, __d1_sram3_start
            │ (+HEAP_D1_SRAM2)       │
            │ (.noinit_d1_sram2)     │
            │ (.bss_d1_sram2)        │  only if D1_SRAM2 exists
  D1_SRAM2  │ (.data_d1_sram2)       │
0x2404 0000 ├────────────────────────┤◄ __d1_sram1_end, __d1_sram2_start
            │  +HEAP_D1_SRAM1        │
            │  .noinit_d1_sram1      │
            │  .bss_d1_sram1         │
  D1_SRAM1  │  .data_d1_sram1        │
0x2400 0000 └────────────────────────┘◄ __d1_sram1_start

            ┌────────────────────────┐◄ __dtcm_end
            │  +HEAP_DTCM            │
            │  .noinit_dtcm          │
            │  .noinit               │
   D-Code   │  .bss_dtcm             │
    only    │  .data_dtcm            │
   access   │  .data                 │
            │  .fastdata             │
            │  +PROCESS_STACK_SIZE   │◄ __process_stack_top
    DTCM    │  +MAIN_STACK_SIZE      │◄ __main_stack_top
0x2000 0000 └────────────────────────┘◄ __dtcm_start

            ┌────────────────────────┐◄ __flash_end
            │        (unused)        │
            ├────────────────────────┤◄ __rom_end
            │  .table.heap           │
            │  .table.copy.extern    │
   tables   │  .table.zero.extern    │
            │  .table.copy.intern    │
            │  .table.zero.intern    │
            │                        │
            │  .data_d3_sram         │
            │ (.data_d2_sram3)       │◄ only if D2_SRAM3 exists
            │  .data_d2_sram2        │
            │  .data_d2_sram1        │
            │ (.data_d1_sram3)       │◄ only if D1_SRAM3 exists
            │ (.data_d1_sram2)       │◄ only if D1_SRAM2 exists
    copy    │  .data_d1_sram1        │
    only    │  .data_dtcm            │
            │  .data_itcm            │
            │  .data_backup          │
            │  .data                 │
            │  .fastdata             │
            │  .fastcode             │
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

            ┌────────────────────────┐◄ __itcm_end
            │  +HEAP_ITCM            │
            │  .noinit_ccm           │
            │  .bss_itcm             │
            │  .data_itcm            │
            │  .fastcode             │
    ITCM    │  .vector_ram           │
0x0000 0000 └────────────────────────┘◄ __itcm_start
```
