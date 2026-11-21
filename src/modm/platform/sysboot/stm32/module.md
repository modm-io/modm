# System Bootloader

Starts the bootloader in the STM32 system memory from the application, for
example to update the firmware via USB DFU, USART, I2C, SPI or CAN without
touching the BOOT0 pin.

```cpp
// Resets the device and starts the bootloader
modm::platform::SystemBootloader::reboot();
```

`reboot()` writes a magic value into a `.noinit` variable and resets the
device. A hardware init hook runs as the very first hook after the reset and,
when it finds the magic value, jumps into the system memory before the clock
tree, the GPIOs or any interrupt is configured. This satisfies the
requirements of the AN2606 application note for jumping to the bootloader from
user code without having to undo the application's hardware configuration.

The vector table address of the bootloader comes from AN2606 Table 226 and the
address of the bootloader ID from the STM32CubeProgrammer database, both via
modm-data in the `sysboot` driver of the device file.

`SystemBootloader::id()` reads the bootloader ID from the system memory, which tells you the supported interfaces and the bootloader
version of the actual silicon, for example `0x92` (USART, CAN, DFU, I2C, SPI,
version 2).

## USB DFU Runtime

With the TinyUSB DFU runtime class (`modm:tinyusb:config` = `device.dfu_rt`),
the host can request the reboot via `dfu-util -e`. Delay the reset so that
TinyUSB can still answer the `DFU_DETACH` request:

```cpp
modm::Timeout dfu_timeout;
void tud_dfu_runtime_reboot_to_dfu_cb() { dfu_timeout.restart(100ms); }

// in the main loop
if (dfu_timeout.execute()) SystemBootloader::reboot();
```

## Limitations

- The STM32F04x, STM32F070x6 and STM32L01x/02x check whether the flash is
  empty and jump right back into the application (AN2606 §16.1, §17.1,
  §63.1). The bootloader address of the STM32G5, STM32H543/553 and STM32H7P is
  not documented. So this module is not available for them, nor for the
  STM32WB0 and STM32WL3 with their different ROM bootloader.
- On the STM32F1 XL-density devices with dual bank boot enabled (BFB2 option
  bit), and on the STM32F7 with nDBOOT or nDBANK, the bootloader may jump into
  bank 2 instead. Disable the dual bank boot option first.
- On devices with TrustZone the jump only works with TZEN=0.
- The bootloader configures its own clocks and leaves some peripherals enabled
  when it exits via the Go command, including the DFU leave request. Reset the
  device to leave the bootloader, except on the STM32H5, see below.

## Leaving the Bootloader on STM32H5

The STM32H5 bootloader exits by jumping to the application instead of resetting
the device. After a start through the BOOT0 pin, the secure firmware (SFSP) has
mapped only the resources of the bootloader into the non-secure domain, so that
"a jump from the BL to an application using other resources does not work"
(AN2606 §4.10, Boot_V3_2). On the STM32H563 the application then hangs during
its startup. So on the STM32H5 this module checks the reset flags first: the
bootloader clears them and every real reset sets at least one, so without a flag
the module resets the device once, before anything else is initialized.

An application that is started by your own bootloader sees the same condition if
that bootloader clears the reset flags. It then starts after at most two extra
resets, which a value in `.noinit` counts, so that this cannot become a loop.
The bootloader may overwrite that value, which only means one more reset.

A wake-up from Standby sets no reset flag either, so the module does not reset
while the `SBF` flag in `PWR_PMSR` is set. This flag survives resets: clear it
with `CSSF` after a wake-up, otherwise the next bootloader exit is not detected.
