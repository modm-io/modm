# FreeRTOS+TCP

Amazon FreeRTOS+TCP is a scalable, open source and thread safe TCP/IP stack for
FreeRTOS. FreeRTOS+TCP's features and RAM footprint are fully scalable, making
it equally applicable to smaller lower throughput microcontrollers as larger
higher throughput microprocessors.

This module provides the latest FreeRTOS+TCP LTS release integrated with modm:

- `FreeRTOS_printf()` and `FreeRTOS_debug_printf()` forwarded to `modm:debug`.


## Configuration

This module generates a `FreeRTOSIPConfig.h` config file for modm integration
with these overwritable settings:

- `ipconfigIP_TASK_STACK_SIZE_WORDS` = `(configMINIMAL_STACK_SIZE * 4)`
- `ipconfigIP_TASK_PRIORITY` = `(configMAX_PRIORITIES - 2)`
- `FreeRTOS_printf(x)` = `MODM_LOG_INFO.printf(x)`
- `FreeRTOS_debug_printf(x)` = `MODM_LOG_DEBUG.printf(x)`

All other config settings are unchanged by modm and are defaulted by FreeRTOS
itself, please refer to the [FreeRTOS+TCP Config documentation][config] to
understand what each of these do.

To change a configuration setting, define a `<FreeRTOSIPConfigLocal.h>` file,
which is included *before* the optional settings, so you can overwrite them
easily.

An example `<FreeRTOSIPConfigLocal.h>` file:

```c
// Change IP task stack size
#define ipconfigIP_TASK_STACK_SIZE_WORDS  1024
// Enable printing
#define ipconfigHAS_PRINTF  1
#define ipconfigHAS_DEBUG_PRINTF  1
```

[config]: https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_IP_Configuration.html
