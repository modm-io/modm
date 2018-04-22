# ARM Cortex-M startup

## Linkerscript

### Placement of `.fastcode` section

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

So for STM32s where the CCM is _not_ connected to the I-Bus, we execute
`.fastcode` from Flash.
