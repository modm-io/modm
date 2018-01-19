# Porting to MODM

## What MODM already supports

MODM contains extensive support for AVR and STM32 devices, specifically:

- AT90, ATtiny, ATmega devices
- STM32 F0, F1, F3, F4, F7, L4 devices.

If your device falls into any of those categories, you can just use the specific device
identifier in your `project.xml` and it'll very likely just work out-of-the-box.

If you want to add a development board for an existing device, have a look at [`modm/platform/board`](https://github.com/modm-io/modm/tree/develop/src/modm/board).

TODO: This guide needs to be updated for MODM.