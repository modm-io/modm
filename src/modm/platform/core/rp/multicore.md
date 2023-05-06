# RP2040 multi-core module

This module adds the ability to run code on CPU1 and provides mailbox, spinlock
and mutex implementations, and additionally a stack of size
`modm:platform:cortex-m:main_stack_size` is added to the CORE1 memory and the
`modm::atomic::Lock` is made multicore safe.
You can use attributes for placing objects into core-coupled memories:

```cpp
modm_core0_data uint8_t data = 0xab;
modm_core1_bss uint16_t zero;
modm_core0_noinit uint32_t uninitialized;

modm_core1_code
void function()
{
    data = 0xcd;
    zero = 100;
    uninitialized = 200;
}
```

We provide a symmetric multiprocessing (SMP) abstraction, where the same binary
is used for both cores. By default CPU0 is used to boot the device and code
running on CPU1 needs to be explicitly started:

```cpp
void function()
{
    uint8_t cpuid = Core::cpuId();
}
function(); // cpuid == 0
multicore::Core1::run(function); // cpuid == 1
// Before CPU1 executes code, this function is called
// so that you can customize the CPU1 environment
extern "C" void modm_initialize_core1()
{
    // set a custom vector table for CPU1
    SCB->VTOR = custom_vector_table;
}
```
