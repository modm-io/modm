# Nordic nRF core module

This module specializes the generic `modm:platform:cortex-m` module with
nRF-specific startup code and linkerscript generation.


## Startup

The `__modm_initialize_platform()` callback performs only early platform setup:

### nRF52

- configures reset pin mapping in UICR (device-dependent),
- disables APPROTECT,
- enables instruction cache (if `modm:platform:core:enable_icache` is enabled
  and supported by the selected target).

### nRF53

- copies trimming values from FICR into their target registers,
- disables APPROTECT (and SECUREAPPROTECT on the app core),
- enables instruction cache (NVMC on net core, CACHE peripheral on app core,
  if `enable_icache` is enabled).


## Linkerscript

nRF currently reuses the generic Cortex-M RAM/Flash linker layout and does not
require a dedicated, family-specific linker template like STM32.

For most targets this yields one continuous RAM region plus per-memory helper
sections that can be targeted explicitly via `modm_section`:

```cpp
// copied from flash at startup
modm_section(".data_ram0")
uint32_t data0 = 0x12345678;

// zero-initialized at startup
modm_section(".bss_ram1")
uint8_t dmaBuffer[1024];

// left uninitialized
modm_section(".noinit_ram2")
uint8_t retained[256];
```
