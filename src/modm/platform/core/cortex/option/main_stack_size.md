# Minimum size of the application main stack

The ARM Cortex-M uses a descending stack mechanism which is placed so that it
grows towards the beginning of RAM. In case of a stack overflow the hardware
then attempts to stack into invalid memory which triggers a HardFault. A stack
overflow will therefore never overwrite any static or heap memory and this
protection works without the MPU and therefore also on ARM Cortex-M0 devices.

If the vector table is relocated into RAM, the start address needs to be aligned
to the next highest power-of-two word depending on the total number of device
interrupts. On devices where the table is relocated into the same memory as the
main stack, an alignment buffer up to 1kB is added to the main stack.

```
|              ...                |
|---------------------------------|
|    Interrupt Vectors (in RAM)   |
|        (if re-mapped)           | <-- vector table origin
|---------------------------------| <-- main stack top
|           Main Stack            |
|       (grows downwards)         |
|               |                 |
|               v                 |
|---------------------------------|
|  Alignment buffer for vectors   |
|   (overwritten by main stack!)  |
'---------------------------------' <-- RAM origin
```
