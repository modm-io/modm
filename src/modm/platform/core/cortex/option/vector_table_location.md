Vector table location in ROM or RAM

The vector table is always stored in ROM and copied to RAM by the startup script
if required. You can modify the RAM vector table using the CMSIS NVIC functions:

- `void NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)`
- `uint32_t NVIC_GetVector(IRQn_Type IRQn)`

For applications that do not modify the vector table at runtime, relocation to
RAM is not necessary and can save a few hundred bytes of static memory.

By default, the fastest option is chosen depending on the target memory
architecture. This does not always mean the table is copied into RAM, and
therefore may not be modifiable with this option!

From the ARM Cortex-M4 Technical Reference Manual on exception handling:

> - Processor state is automatically stored to the stack on an exception, and
>   automatically restored from the stack at the end of the Interrupt Service
>   Routine.
> - The vector is fetched in parallel to the state saving, enabling efficient
>   interrupt entry.

!!! warning
    Placing main stack and vector table into the same memory can significantly
    slow down interrupt latency, since both I-Code and D-Code memory interface
    need to fetch from the same access port.
