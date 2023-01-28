# Vector table location in ROM or RAM on F0 devices

STM32 devices with a Cortex-M0 core provide a platform-specific method to place
the interrupt vector table in SRAM although the core does not support vector
table relocation. It is only available on STM32F0 since all other devices
can remap the vector table in the Cortex-M core.

When this method is activated the vector table is copied to the start of SRAM1
by the startup script. The `SYSCFG->CFGR1` register is set to remap the
beginning of SRAM to the vector table location at `0x0000 0000`.

You can modify the RAM vector table using the CMSIS NVIC functions:

- `void NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)`
- `uint32_t NVIC_GetVector(IRQn_Type IRQn)`

This remapping method allows to easily boot an application from a custom
bootloader even if the Cortex-M0 core does not support relocation.

For applications that do not modify the vector table at runtime, relocation to
RAM is not necessary and can save a few hundred bytes of static memory.

!!! warning "On Interrupt Latency"
    Placing main stack and vector table into the same memory can significantly
    slow down interrupt latency, since both I-Code and D-Code memory interface
    need to fetch from the same access port.
