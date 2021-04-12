# Explore the Examples

To get a quick feel about modm's APIs you can look at and experiment
[with our examples][examples], especially if you have a development board that
modm supports out-of-box.

Make sure you've [installed all tools required for building modm][installation]
and you've cloned the modm repository *recursively*:

```sh
git clone --recurse-submodules --jobs 8 https://github.com/modm-io/modm.git
```


## TL;DR

Change directory into any of the examples and compile it:

```sh
# cd into any example you like
cd modm/examples/generic/blinky
# generate modm library (call only once)
lbuild build
# compile the example
scons -j8
# Connect your development board and upload the firmware
scons program
```

To debug with GDB in TUI mode. Make sure to change to the debug profile:

```sh
# compile and upload debug profile
scons program profile=debug
# launch OpenOCD and GDB for debugging
scons debug profile=debug
```

To generate your target specific Doxypress documentation:

```sh
(cd modm/docs && doxypress doxypress.json)
# open modm/docs/html/index.html
```

To remove it all:

```sh
# Remove build artifacts
scons -c
# Remove generated files
lbuild clean
```

Have a look at the [build system documentation][build_docs] and the
[online documentation][docs].


## Interesting Examples

We have hundreds of examples but here are some of our favorite ones for our
supported development boards:

<!--checkrepourls-->
- Arduino Uno:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/arduino_uno/basic/blink/main.cpp),
[Button & Serial](https://github.com/modm-io/modm/blob/develop/examples/arduino_uno/basic/digital_read_serial/main.cpp),
[Analog & Serial](https://github.com/modm-io/modm/blob/develop/examples/arduino_uno/basic/read_analog_voltage/main.cpp).
- NUCLEO-F031K6:
[Blinky & Serial](https://github.com/modm-io/modm/tree/develop/examples/nucleo_f031k6/blink/main.cpp).
- NUCLEO-F103RB:
[Blinky & Serial](https://github.com/modm-io/modm/blob/develop/examples/nucleo_f103rb/blink/main.cpp),
[Undefined IRQ Handler](https://github.com/modm-io/modm/blob/develop/examples/nucleo_f103rb/undefined_irq/main.cpp),
[Logging via ITM](https://github.com/modm-io/modm/blob/develop/examples/nucleo_f103rb/itm/main.cpp).
- NUCLEO-F429ZI:
[CMSIS DSP](https://github.com/modm-io/modm/tree/develop/examples/nucleo_f429zi/cmsis_dsp).
- NUCLEO-F446RE:
[Internal Flash Programming](https://github.com/modm-io/modm/blob/develop/examples/nucleo_f446re/flash/main.cpp).
- STM32F072 Discovery:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/stm32f072_discovery/blink/main.cpp),
[CAN](https://github.com/modm-io/modm/blob/develop/examples/stm32f072_discovery/can/main.cpp),
[Gyroscope](https://github.com/modm-io/modm/blob/develop/examples/stm32f072_discovery/rotation/main.cpp).
- STM32F3 Discovery:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/blink/main.cpp),
[CAN](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/can/main.cpp),
[Accelerometer](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/accelerometer/main.cpp),
[Gyroscope](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/rotation/main.cpp),
[TinyUSB DFU](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/usb_dfu/main.cpp),
[Logging via RTT](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/rtt/main.cpp).
- STM32F4 Discovery:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/blink/main.cpp),
[CAN](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/can/main.cpp),
[Accelerometer](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/accelerometer/main.cpp),
[Timer & LED Animations](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/timer/main.cpp).
- STM32F469 Discovery:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/stm32f469_discovery/blink/main.cpp),
[Drawing on display](https://github.com/modm-io/modm/blob/develop/examples/stm32f469_discovery/display/main.cpp),
[Touchscreen inputs](https://github.com/modm-io/modm/blob/develop/examples/stm32f469_discovery/touchscreen/main.cpp),
[Multi-heap with external 16MB memory](https://github.com/modm-io/modm/blob/develop/examples/stm32f469_discovery/tlsf-allocator/main.cpp),
[Game of Life in Color with Multitouch](https://github.com/modm-io/modm/tree/develop/examples/stm32f469_discovery/game_of_life/main.cpp),
[Hard Fault with CrashCatcher](https://github.com/modm-io/modm/tree/develop/examples/stm32f469_discovery/hard_fault/main.cpp)
- STM32F769 Discovery:
[FPU with double precision](https://github.com/modm-io/modm/blob/develop/examples/stm32f769i_discovery/blink/main.cpp)


Here are some additional examples of displays and sensors we like:

- [SSD1306 OLED display](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/display/ssd1306/main.cpp): Draws text and graphics onto I2C display.
- [BMP085/BMP180 barometer](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/barometer_bmp085_bmp180/main.cpp): Reads atmospheric pressure and temperature from I2C sensor.
- [BMP180/BME280 barometer](https://github.com/modm-io/modm/tree/develop/examples/stm32f103c8t6_blue_pill/environment): Reads atmospheric pressure and temperature from multiple I2C sensors.
- [VL6180 time-of-flight distance sensor](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/distance_vl6180/main.cpp): Reads distance and ambient light from I2C sensor.
- [VL53L0  time-of-flight distance sensor](https://github.com/modm-io/modm/tree/develop/examples/nucleo_f401re/distance_vl53l0/main.cpp): Much improved version of the VL6180 sensor.
- [ADNS9800 motion sensor](https://github.com/modm-io/modm/tree/develop/examples/stm32f103c8t6_blue_pill/adns_9800/main.cpp): Reads 2D motion from SPI sensor used in gaming mice.
- [TCS3414 color sensor](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/colour_tcs3414/main.cpp): Reads RGB color from I2C sensor.
- [HD44780 over I2C-GPIO expander](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/display/hd44780/main.cpp): Draws text via native GPIO port or I2C-GPIO expander port onto character display.

<!--/checkrepourls-->


### Copy Carefully

When copying from our examples make sure to set the repository path correctly!
All example `modm/examples/**/project.xml` files are missing this path, since we
set it in the inherited base `modm/examples/lbuild.xml` configuration.
You must also add the `modm:docs` module manually if you want it.

The `modm/examples/lbuild.xml` file:

```xml
<library>
  <!-- This is the default lbuild configuration file for every
       example in this folder. It only defaults the common settings,
       so that this isn't duplicated in every single example.
       When you write your own application, you must set this
       path yourself! -->
  <repositories>
  	<repository><path>../repo.lb</path></repository>
  </repositories>
  <modules>
  	<module>modm:docs</module>
  </modules>
</library>
```

[examples]: https://github.com/modm-io/modm/tree/develop/examples
[installation]: https://modm.io/guide/installation
[make]: https://modm.io/reference/module/modm-build-make
[build_docs]: https://modm.io/reference/build-systems/
[docs]: https://modm.io/reference/documentation/
