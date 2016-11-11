Minimum size of the application main stack

The ARM Cortex-M uses a descending stack mechanism which is placed so that it
grows towards the beginning of RAM. In case of a stack overflow the hardware
then attempts to stack into invalid memory which triggers a HardFault. A stack
overflow will therefore never overwrite any static or heap memory and this
protection works without the MPU and therefore also on ARM Cortex-M0 devices.

If you enable either the LED or the logging HardFault option, a smaller stack
is added above the main stack. This stack is only used by the HardFault handler
when not enough memory remains in the main stack to preserve GDB backtrace
behavior. This memory also acts as a small safety buffer against main stack
underflow, which is not detected however.

If the vector table is relocated into RAM, the start address needs to be aligned
to the next highest power-of-two word depending on the total number of device
interrupts. On devices where the table is relocated into the same memory as the
main stack, an alignment buffer up to 1kB is added to the main stack.

!!! warning
    The main stack size you provide is a minimum and may be enlarged to satisfy
    alignment requirements. Be aware that these requirements operate on the sum
    of HardFault and main stack. Disabling HardFault options may therefore
    decrease the alignment buffer added to the main stack size, which may make
    your application overflow stack. You need to increase your minimum main
    stack size in that case.

!!! note
    The main stack is watermarked and you can get the maximum stack usage using
    the `uint32_t xpcc::cortex::getMaximumStackUsage()` function.

    |              ...                |
    |---------------------------------|
    |    Interrupt Vectors (in RAM)   |
    |        (if re-mapped)           | <-- vector table origin
    |---------------------------------| <-- HardFault stack top
    |        HardFault Stack          |
    |       (grows downwards)         |
    |               |                 |
    |               v                 |
    |---------------------------------| <-- main stack top
    |           Main Stack            |
    |       (grows downwards)         |
    |               |                 |
    |               v                 |
    |---------------------------------|
    |  Alignment buffer for vectors   |
    |   (overwritten by main stack!)  |
    '---------------------------------' <-- RAM origin
