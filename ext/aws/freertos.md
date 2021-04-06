# FreeRTOS

Amazon FreeRTOS is an open source, real-time operating system (RTOS) for
microcontrollers in small, low-power devices.

This module provides the latest FreeRTOS LTS release integrated with modm:

- Chooses the right Cortex-M port for the target.
- Variable tick rate integrated with `modm::Clock` via `modm:platform:clock`.
- Dynamic memory provided by the `modm:platform:heap` module with thread-safe
  locking of Newlib's `malloc/free`.
- Assertions integrated into the `modm:architecture:assert` interface.
- Interrupt handling integrated with CMSIS NVIC functions.

Note that we recommend using the FreeRTOS API directly and only if you write
threaded code that also runs on other platforms (x86 simulators for example)
should you consider using the API defined in `modm:processing:rtos` module.


## Configuration

This module generates a `FreeRTOSConfig.h` config file for modm integration with
these settings:

- `configASSERT(x)` implemented with `modm_assert(x, "freertos")`.
- `configCPU_CLOCK_HZ` implemented with CMSIS `SystemCoreClock`.
- `configTICK_RATE_HZ` set to `modm:freertos:frequency` or 1kHz on Cortex-M0.
- `configSUPPORT_DYNAMIC_ALLOCATION = 1` as implemented by `modm:platform:heap`.
- `configUSE_TICK_HOOK = 1` used by `modm:platform:clock` to provide `modm::Clock`.

In addition we define these overwritable default settings:

- `configMAX_SYSCALL_INTERRUPT_PRIORITY` = `(__NVIC_PRIO_BITS << (8u - __NVIC_PRIO_BITS))`

- `configMINIMAL_STACK_SIZE` = 128
- `configMAX_PRIORITIES` = 5
- `configUSE_PREEMPTION` = 1
- `configUSE_IDLE_HOOK` = 0
- `configUSE_16_BIT_TICKS` = 0

- `configUSE_MUTEXES` = 1
- `configUSE_RECURSIVE_MUTEXES` = 1
- `configUSE_COUNTING_SEMAPHORES` = 1
- `configQUEUE_REGISTRY_SIZE` = 10
- `configNUM_THREAD_LOCAL_STORAGE_POINTERS` = 5
- `configCHECK_FOR_STACK_OVERFLOW` = 1

- `configUSE_TIMERS` = 1
- `configTIMER_TASK_PRIORITY` = 3
- `configTIMER_QUEUE_LENGTH` = 10
- `configTIMER_TASK_STACK_DEPTH` = `configMINIMAL_STACK_SIZE`

All other config settings are unchanged by modm and are defaulted by FreeRTOS
itself, please refer to the [FreeRTOS Config documentation][config] to
understand what each of these do.

To change a configuration setting, define a `<FreeRTOSConfigLocal.h>` file,
which is included *before* the optional settings, so you can overwrite them
easily.

An example `<FreeRTOSConfigLocal.h>` file:

```c
// Use a bigger minimal stack size
#define configMINIMAL_STACK_SIZE  256
// Longer timer queue
#define configTIMER_QUEUE_LENGTH  20
```

[config]: https://www.freertos.org/a00110.html
