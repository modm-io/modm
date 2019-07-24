# Examples, glorious Examples!

<!--examples-->
The easiest way for you to learn about modm's APIs is to look at and experiment
[with our examples](https://github.com/modm-io/modm/tree/develop/examples),
especially if you have a development board that modm supports out-of-box.

Make sure you've [installed all tools required for building modm](https://modm.io/guide/installation).


## TL;DR

```
lbuild build    # generate modm library (call only once)
scons program   # compile and upload to your development board
```

To debug with GDB in TUI mode:

```
scons program profile=debug # compile and upload debug profile
scons gdb profile=debug     # launch OpenOCD and GDB for debugging
```

To generate your target specific Doxygen documentation:

```
(cd modm/docs && doxygen doxyfile.cfg)  # may take a minute or two
# open modm/docs/html/index.html
```

To remove it all:

```
scons -c      # removes build artifacts
lbuild clean  # removes generated files
```


## Quickstart

To generate the modm library for the specific example target, call

```
lbuild build
```

You can then look at the generated modm code in the local `modm/src/modm` folder.

Most of our examples compile with SCons by default, however you can generate
a CMake build script by including the [CMake build script generator module](https://modm.io/reference/module/modm-build-cmake).

```
lbuild build -m "::cmake"
```


To compile the example and modm, call SCons or CMake:

```
scons
# or for CMake
make cmake           # call just once
make build-release
```

To upload the example to your development board:

```
scons program
# or for CMake
make upload-release
```

You can also debug your examples. Make sure you've compiled and uploaded the
debug profile first, because debugging a release profile is annoying:

```
scons program profile=debug
# or for CMake
make upload-debug
```

Then just do this to open up GDB in TUI mode:

```
scons gdb profile=debug
# or for CMake
make openocd
# open another shell to this location
make gdb
```


## Interesting Examples

We have a lot of examples, <!--examplecount-->190<!--/examplecount--> to be
exact, but here are some of our favorite examples for our supported development
boards:

<!--checkrepourls-->
- Arduino Uno:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/arduino_uno/basic/blink/main.cpp),
[Button & Serial](https://github.com/modm-io/modm/blob/develop/examples/arduino_uno/basic/digital_read_serial/main.cpp),
[Analog & Serial](https://github.com/modm-io/modm/blob/develop/examples/arduino_uno/basic/read_analog_voltage/main.cpp).
- NUCLEO-F031K6:
[Blinky & Serial](https://github.com/modm-io/modm/tree/develop/examples/nucleo_f031k6/blink/main.cpp).
- NUCLEO-F103RB:
[Blinky & Serial](https://github.com/modm-io/modm/blob/develop/examples/nucleo_f103rb/blink/main.cpp),
[Debugging hard faults](https://github.com/modm-io/modm/blob/develop/examples/nucleo_f103rb/hard_fault/main.cpp).
- STM32F072 Discovery:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/stm32f072_discovery/blink/main.cpp),
[CAN](https://github.com/modm-io/modm/blob/develop/examples/stm32f072_discovery/can/main.cpp),
[Gyroscope](https://github.com/modm-io/modm/blob/develop/examples/stm32f072_discovery/rotation/main.cpp).
- STM32F3 Discovery:
[Blinky](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/blink/main.cpp),
[CAN](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/can/main.cpp),
[Accelerometer](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/accelerometer/main.cpp),
[Gyroscope](https://github.com/modm-io/modm/blob/develop/examples/stm32f3_discovery/rotation/main.cpp).
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
[Game of Life in Color with Multitouch](https://github.com/modm-io/modm/tree/develop/examples/stm32f469_discovery/game_of_life/main.cpp)
- STM32F769 Discovery:
[FPU with double precision](https://github.com/modm-io/modm/blob/develop/examples/stm32f769i_discovery/blink/main.cpp)


Here are some additional examples of displays and sensors we like:

- [SSD1306 OLED display](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/oled_display/main.cpp): Draws text and graphics onto I2C display.
- [BMP085/BMP180 barometer](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/barometer_bmp085_bmp180/main.cpp): Reads atmospheric pressure and temperature from I2C sensor.
- [BMP180/BME280 barometer](https://github.com/modm-io/modm/tree/develop/examples/stm32f103c8t6_blue_pill/environment): Reads atmospheric pressure and temperature from multiple I2C sensors.
- [VL6180 time-of-flight distance sensor](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/distance_vl6180/main.cpp): Reads distance and ambient light from I2C sensor.
- [VL53L0  time-of-flight distance sensor](https://github.com/modm-io/modm/tree/develop/examples/nucleo_f401re/distance_vl53l0/main.cpp): Much improved version of the VL6180 sensor.
- [ADNS9800 motion sensor](https://github.com/modm-io/modm/tree/develop/examples/stm32f103c8t6_blue_pill/adns_9800/main.cpp): Reads 2D motion from SPI sensor used in gaming mice.
- [TCS3414 color sensor](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/colour_tcs3414/main.cpp): Reads RGB color from I2C sensor.
- [HD44780 over I2C-GPIO expander](https://github.com/modm-io/modm/blob/develop/examples/stm32f4_discovery/display/hd44780/main.cpp): Draws text via native GPIO port or I2C-GPIO expander port onto character display.

<!--/checkrepourls-->
<!--/examples-->
