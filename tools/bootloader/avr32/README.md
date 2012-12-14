avr32_dfu_config.py

Copyright 2012 strongly-typed

Configuration Utility for pre-programmed DFU USB Bootloader
===========================================================

This tool can be used to create the bootloader configuration
in the user page of the flash.

Most (all?) Atmel AVR32 UC3 devices with USB come with a preprogrammed
USB bootloader that supports the DFU protocol. See
(http://www.atmel.com/Images/doc7745.pdf)
for details about the bootloader.

The behaviour of the bootloader can be configured with a 32-bit word
at the end of the flash user page.

The factory default is that the bootloader always starts. To start
the application a programming tool like dfu-programmer must be used.
This is very inconvenient if the application should start automatically.

It is possible to configure the bootloader with the ISP_IO_COND_EN fuse.
If the fuse is set to '1' and the configuration word in the user
page is valid the bootloader will only start if the configured GPIO
pin has the configured level. Otherwise the application is started.

Usage:

1) Add a button to your design and connect it to any GPIO of the device.
   Connect the button between the GPIO and GND. A pull-up to VCC is necessary.

2) Test if the GPIO works correctly.
   Write a simple application to that lights up a LED if your button is pressed.
   Once you configured the bootloader to check the GPIO pin you need
   a JTAG adaptor to change the firmware again if your button does not work.

3) Create the configuration using this tool.
   Select the GPIO number (e.g. 19) and the active level (--low or --high) of your GPIO.
   If your button connects to GND the active level is --low.
   $ python avr32_dfu_config.py --low 19

4) Write the user page to the device
   $ dfu-programmer at32uc3b0256 erase
   $ dfu-programmer at32uc3b0256 flash-user userpage.hex

5) Check the if the user page is written correctly
   $ dfu-programmer at32uc3b0256 dump-user | hexdump
   The last four bytes should be the bytes avr32_dfu_config.py displayed.

6) Change the ISP_IO_COND_EN fuse to '1'
   $ dfu-programmer at32uc3b0256 setfuse ISP_IO_COND_EN=1
   $ dfu-programmer at32uc3b0256 reset
   $ dfu-programmer at32uc3b0256 reset

7) If the button is not pressed during reset your application should start
   directly. If the button is pressed the bootloader should start.

Enjoy programming AVR32 without any additional expensive hardware tools!
