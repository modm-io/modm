# Porting to xpcc

## What xpcc already supports

xpcc contains extensive support for AVR and STM32 devices, specifically:

- AT90, ATtiny, ATmega devices
- STM32 F0, F1, F3, F4, F7 devices.

If your device falls into any of those categories, you can just use the specific device
identifier in your `project.cfg` and it'll very likely just work out-of-the-box.

If you want to add a development board for an existing device, have a look at [`xpcc/architecture/platform/board`](https://github.com/roboterclubaachen/xpcc/tree/develop/src/xpcc/architecture/platform/board).

## Adding support for new AVR or STM32 devices

It may be as easy as adding a new device file, but if it's a brand new device, [the device headers also need to be updated](https://github.com/roboterclubaachen/xpcc/tree/develop/ext/). When porting a new series (for example the STM32L4), a lot of existing code can be reused, but there may be very important differences between the two that are not immediately obvious to someone who isn't that familiar with STM32 devices.

We recommend [writing us an email][mailing_list], or [opening up an issue on GitHub](https://github.com/roboterclubaachen/xpcc/issues).

## Porting a new device from a new vendor

Note: We assume this new device is Cortex-M based. For other CPU architectures, [send us an email][mailing_list].

Porting to a new vendor device requires you to:

1. Add a bunch of header files for the new device.
2. Add a device files for the new device.
3. Make the build system aware of the new device classes.
4. Adapt the Cortex-M startup- and linkerscripts.
5. Compile a minimal example.
6. Write a basic GPIO and delay driver.
7. Compile an LED blinking example.

We'll discuss every point in detail for the rest of this chapter.

### Adding device header files

Copy the [CMSIS header files](https://www.arm.com/products/processors/cortex-m/cortex-microcontroller-software-interface-standard.php)  for the device family into the `ext/` folder. You can usually download them as part of a driver pack from the vendor's website.
Have a look at [the STM32 device headers](https://github.com/roboterclubaachen/xpcc/tree/develop/ext/st) as an example for what you are looking for.

Create a `README.md` that contains information on the source of the header files, with a link to the vendor website, the download date/version and most importantly the license text.
Don't forget to add the include path of the device files to the [`ext/SConstruct`](https://github.com/roboterclubaachen/xpcc/blob/develop/ext/SConscript) file!


### Adding device files

**Please don't manually create device files for STM32 devices.** We generate these files from raw vendor data. Please ping [@salkinium](https://github.com/salkinium) if a device file for your STM32 is missing.

The [xpcc device files](https://github.com/roboterclubaachen/xpcc/tree/develop/src/xpcc/architecture/platform/devices) contain the information that is used to generate the HAL. For AVR and STM32 devices these files are generated from another (vendor specific) data source.
However, for a new vendor such machine readable data might not be available and if it is, it will require some work to create the necessary parser to automatically generate device files.
We recommend building a minimal proof-of-concept port for one device first.
Once you have one device working, you can get started working on an automatic parser if you want to support a wide range of devices.

You need to manually create a new device file for your device. We recommend copying [this device file for the STM32F303](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/devices/stm32/stm32f303-c_k_r-6_8.xml) to a new folder named after the platform (like `devices/sam`).

Vendors usually have a naming convention of their devices, make sure the device attributes and file name reflect this. The semantics of the vendor's device naming scheme can sometimes be gathered directly from the datasheet or by comparing different devices (i.e. their names and capabilities).
For example, the `SAM4LC4B` identifier can be split like this:
```xml
  <device platform="sam" family="4" name="lc" pin_id="b" size_id="4">
    <!-- ... -->
  </device>
```

Modify the copied device file to reflect the new devices data.
Since you only have one specific device to port, you can throw out the duplicate XML tags which are normally filtered by `device-` attributes.

Adding the total FLASH and RAM sizes allows the build system to display accurate static resource usage and to warn you if your program does not fit into memory:
```xml
<flash>65536</flash>
<ram>16384</ram>
```

Choose the correct ARM Cortex-M core (`m0`, `m3`, `m4`, `m4f`, `m7`, `m7f`, `m7fd`):
```xml
<core>cortex-m4f</core>
```

The CMSIS device headers you just added are now available in the include path, so you should add the device header:
```xml
<header>sam4.h</header>
```

Often you must not include the device specific device header directly, but let the family device header choose it for you based on a preprocessor define, which you can provide here too:
```xml
<define>SAM4LC4B</define>
```

You need to include at least three drivers: `core`, `clock` and `gpio`.

Make sure you give the `core` driver the right information about the memory map. This information is used to generate the linkerscript.
Don't worry about different memories right now, just add the main FLASH and one SRAM region here (called `sram1`). The `size` unit is kB.
```xml
<driver type="core" name="cortex">
  <memory access="rx" start="0x8000000" name="flash" size="64"/>
  <memory access="rwx" start="0x20000000" name="sram1" size="12"/>
```

Add all or part of the vector table. No need to add the Cortex-M exceptions, we do that for you.
The `name` will be appended with `_Handler`, so declaring `extern "C" void WWDG_Handler(void) {...}` anywhere will link that function into the vector table (note the C linkage!).
```xml
  <vector position="0" name="WWDG"/>
  <vector position="1" name="PVD"/>
  <!-- ... -->
  <vector position="81" name="FPU"/>
</driver>
```

The `clock` driver just needs to be included, we'll hardcode the specifics in code:
```xml
<driver type="clock" name="sam4"/>
```

The `gpio` driver should contain some GPIOs, just add the ones you need. Adding more later is cheap. Ignore the alternate functions for now, you only want to blink an LED.
```xml
<driver type="gpio" name="sam4">
  <gpio port="A" id="0"/>
  <gpio port="A" id="1"/>
<driver>
```

Remove all other drivers.

### Making the build system aware

Our build system is organically grown, er, I mean, *has* organically grown. Therefore it is a bit hacky in places and you now need to make it aware of the new devices from a new vendor:

1. [Split the device identifier into the vendor naming scheme useful](https://github.com/roboterclubaachen/xpcc/blob/7b32f74cc43c7bfaf815287902ef2bd4a6baa84d/tools/device_files/device_identifier.py?ts=4#L86-L95).
2. [Find the device file using the naming scheme](https://github.com/roboterclubaachen/xpcc/blob/3c7cd31e5aad66e47d073dc445551a21416d21eb/scons/site_tools/platform_tools.py?ts=4#L92-L103).
3. [Thou Shalt ~~Not~~ Pass](https://github.com/roboterclubaachen/xpcc/blob/7b32f74cc43c7bfaf815287902ef2bd4a6baa84d/tools/device_files/device.py#L134-L139).
4. [Add some platform-specific tests for the new devices](https://github.com/roboterclubaachen/xpcc/blob/3c7cd31e5aad66e47d073dc445551a21416d21eb/scons/site_tools/platform_tools.py?ts=4#L385-L414). These tests are [custom Jinja2 filters](http://jinja.pocoo.org/docs/dev/api/#custom-filters) that can conveniently be used in the driver templates.

### Adapt the startup- and linkerscripts

The [Cortex-M startup script](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/core/cortex/startup.c.in?ts=4) is pretty universal. These are the steps it does:

1. Generate the exception table and places it at the beginning of Flash.
2. Call platform specific code to enable internal memories (**needs porting**).
3. Fill the stack with magic values to catch unintentional stack execution.
4. Copy and zero internal memories.
5. Initialize the Cortex-M CPU.
6. Call `xpcc_gpio_enable()` to enable clock to GPIO (**needs porting**).
7. Call external hardware initialize hook.
8. Copy and zero external memories.
9. Initialize heap on all memories.
10. Call libc init incl. execution of C++ static constructors.
11. Call `main()`.

We'll write the `xpcc_gpio_enable()` function when we port the GPIO driver.

To provide code for step 2 adapt [the platform macro file](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/core/cortex/platform.macros?ts=4).
Add whatever code is needed to initialize the internal memories. Don't do complicated stuff here, the system is not fully booted yet!

The [simplest linkerscript available is `linkerscript/stm32_ram.ld.in`](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/core/cortex/linkerscript/stm32_ram.ld.in?ts=4), which fits everything into one SRAM region.
The linkerscipt diagram in the comment gives you an overview of what goes where, check that against the devices memory map. Note that while the diagram shows specific addresses, the section addresses and sizes are taken from the device file!

You need to tell the [core driver which linkerscript to use for you device](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/core/cortex/driver.xml?ts=4#L29-L40):
```xml
<template device-platform="sam" out="linkerscript.ld">linkerscript/stm32_ram.ld.in</template>
```

### Compile a minimal example

At this point you should test your changes to the build system and the port of the core driver.
Create an minimal example in `xpcc/examples/sam4/minimal`, containing:

- the default `SConstruct` file:
```python
# path to the xpcc root directory
xpccpath = '../../..'
# execute the common SConstruct file
execfile(xpccpath + '/scons/SConstruct')
```

- a minimal `main.cpp` file:
```cpp
/* Copyright (c) {{ year }}, {{ your name }}
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
#include <xpcc/architecture/platform.hpp>

// Leave this function empty for now
extern "C" void xpcc_gpio_enable(void) {}

int main()
{
    // Initialize one GPIO output manually using the datasheet.
    // Don't forget to enable the GPIO clock!
    // GPIO_PORTA_OUTPUT = (1 << 0);

    while(1)
    {
        // Toggle the pin manually
        // GPIO_PORTA_TOGGLE = (1 << 0);

        // Create a very crude busy wait. The numbers don't have to
        // be super accurate, just in the ball-park of a 1s delay.
        // Use the initial CPU frequency in MHz here.
        constexpr uint32_t fcpu_MHz = 8;
        for (volatile uint32_t ii = 0; ii < fcpu_MHz * 250000; ii++) ;
    }
}
```

- the `project.cfg` file:
```ini
[build]
device = sam4lc4b
buildpath = ${xpccpath}/build/sam4/${name}
```

To compile this example, `cd` into the folder and execute `scons`.
If everything goes well, there will be warnings about the missing `clock` and `gpio` drivers, but at the end there should be a printout of the resource usage, similar to this (but with realistic numbers):
```sh
cd examples/sam4/minimal
scons
[...]
Memory Usage
------------
Device: sam4lc4b

Program:    1000 bytes (1.0% used)
(.data + .reset + .rodata + .text)

Data:       1000 bytes (2.0% used) = 1 bytes static (1.0%) + 1000 bytes stack (1.0%)
(.bss + .data + .noinit + .stack)

Heap:      9999 bytes (97.0% available)
(.heap1)
```

You can explore the generated code for the core driver in `xpcc/build/sam4/minimal/libxpcc/generated_platform/driver/core/cortex/`.

If your development board is natively supported via OpenOCD (any of the scripts inside `openocd/share/openocd/scripts/`), you can just add it to your `project.cfg` file:
```ini
[openocd]
configfile = board/atmel_sam4l8_xplained_pro.cfg
```
For custom setups, [read this](http://xpcc.io/reference/build-system/#project-configuration).

You should now be able to program the device using `scons program`.
Check if the LED is blinking at a constant interval.

If the LED is not blinking and you have OpenOCD connected, you can [launch a GDB debug session](https://github.com/roboterclubaachen/xpcc/tree/develop/examples/stm32f3_discovery/gdb).

If something goes wrong in the building step, check the previous steps, maybe you missed something, or we forgot to add something to this guide.
If you're totally stuck [write us an email][mailing_list].

### Porting the delay functions

To keep it simple, we'll leave the device running on the clock it boots up on. This is usually some internal oscillator in the MHz range.
In order to enable the accurate busy-wait delay functions (`xpcc::delay{Nano|Micro|Milli}seconds(uint16_t)`) you need to provide some information about the frequency that the CPU core is running at.

- `fcpu` contains the CPU frequency in Hz
- `fcpu_kHz` contains the CPU frequency in kHz
- `fcpu_MHz` contains the CPU frequency in MHz
- `ns_per_loop` contains the ns per busy-wait delay loop (processor dependent)

Create two files in the `xpcc/architecture/platform/driver/clock/sam4` folder:

- `clock.hpp` (see [STM32 version for verbatim](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/clock/stm32/clock.hpp.in?ts=4)):
```cpp
/* Copyright (c) {{ year }}, {{ your name }}
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
#include <stdint.h>
#include "../../../device.hpp"
#include "../generic/common_clock.hpp"

using namespace xpcc::clock;
```

- `clock.cpp` ([STM32 version](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/clock/stm32/clock.cpp.in?ts=4)) (assuming 8 MHz initial clock):
```cpp
/* Copyright (c) {{ year }}, {{ your name }}
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
#include "clock.hpp"

namespace xpcc
{
namespace clock
{

uint32_t xpcc_fastdata fcpu(8000000);
uint32_t xpcc_fastdata fcpu_kHz(8000);
uint16_t xpcc_fastdata fcpu_MHz(8);
// Cortex-M0: 4000 loops per MHz
// Cortex-M7: 1000 loops per MHz
// otherwise: 3000 loops per MHz
uint16_t xpcc_fastdata ns_per_loop(3000 / 8);

}
}
```

If you [look at the STM32 `driver.xml` file](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/clock/stm32/driver.xml#L19-L22), you'll see that the internal high- and low-speed oscillator values differ even within the same platform. These values are therefore initialized in the clock driver and not the core driver.

### Porting the GPIO driver

In xpcc, GPIOs are assumed to be usable without explicitly initializing the subsystem.
For other peripheral IP, the clock is enabled by the `initialize` method that needs to be explicitly called by the user.

You need to implement the real [`xpcc_gpio_enable()` function here](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/gpio/stm32/gpio_enable.cpp.in), that enables the clocks to all GPIOs on this chip.
```cpp
extern "C"
void
xpcc_gpio_enable(void)
{
    // do your magic
}
```

The GPIO driver is the most important driver of them all, because it gives you bit-banged versions of SPI and I2C through the `xpcc::SoftwareSpiMaster` and `xpcc::SoftwareI2cMaster` classes.

The [STM32 GPIO driver is quite complicated](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/platform/driver/gpio/stm32/gpio.hpp.in?ts=4), but the only methods need to implement right now are [these ones from the GPIO interface](https://github.com/roboterclubaachen/xpcc/blob/develop/src/xpcc/architecture/interface/gpio.hpp?ts=4#L167-L246).

Use the STM32 driver as a template, and modify it. For now you can ignore anything to do with interrupts, analog, alternate functions, `connect` methods, channels, triggers and ports.
It is recommended that you implement the `configure`, `setOutput`, `setInput` methods for `InputType` and `OutputType`, so that you can put the GPIO into open drain mode for I2C.

All GPIO classes are generated by passing in the information of the device file into the [Jinja2 template engine](http://jinja.pocoo.org/docs/dev/templates/#list-of-control-structures), which generates C++ code. In fact, the entire xpcc HAL is build around this principle.

The children of the `<driver type="gpio" ...>` node from the device file are available as a name-mangled dictionary, with `<gpio port="A" ...>` attributes available as their members.
So the `gpio` children are places in the `gpio` + `s` = `gpios` dictionary, which can be accessed by iterating over it:
```jinja2
%% for gpio in gpios
    %# Access `port` attribute:
    %% set port = gpio.port
    %# Access `id` attribute:
    %% set pin = gpio.id
%% endfor
```

Remember, you can always check the generated code for this driver in `xpcc/build/sam4/minimal/libxpcc/generated_platform/driver/gpio/sam4/`

### Blinking an LED

Copy your working `minimal` example and modify the main function to this:
```cpp
/* Copyright (c) {{ year }}, {{ your name }}
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
#include <xpcc/architecture/platform.hpp>

using namespace xpcc::sam4;

using Led = GpioA1;

int main()
{
    Led::setOutput();

    while(1)
    {
        Led::toggle();
        xpcc::delayMilliseconds(500);
    }

    return 0;
}
```

After compilation and programming, the the LED should now be blinking at a 1 second interval.

### What's next?

Please create a PR with this work, so we can review it and discuss the next steps.
Please include all the sources that you used for your port as well as the associated licenses. All code that does not reside in `xpcc/ext` must be licensed under BSD license.

Note that adding full support for all interfaces is a huge task, and won't happen over night. But now that you've got a blinking LED, you've had a successful crash course through our build system, the Cortex-M boot up sequence and the basic GPIO driver. That's a very good start, since this stuff isn't easy.

Thanks for porting!


[mailing_list]: http://mailman.rwth-aachen.de/mailman/listinfo/xpcc-dev
