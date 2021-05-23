# Instrumentation Trace Macrocell (ITM)

This module provides the ITM port 0 as a (buffered) Uart interface.
You can use this class as a *transmit-only* alternative for logging:

```cpp
modm::IODeviceWrapper<modm::platform::Itm,
                      modm::IOBuffer::DiscardIfFull> itm_device;
modm::IOStream stream(itm_device);

modm::platform::Itm::initialize();
stream << "Hello World" << modm::endl;
```


## Caveats

The ITM is part of the ARM Cortex-M debug functionality and forwards its data
to the Trace Port Interface Unit (TPIU) which outputs over the trace pins.
Only the SWO trace pin is currently supported (asynchronous tracing).

!!! warning "The ITM is not a UART replacement!"
    The TPIU only outputs data when a debugger is attached to the target!

The `Itm` class will safely discard any data if a debugger is not attached, so
you can use it for debugging without running the risk of a deadlock.

!!! warning "The ITM requires polling!"
    The hardware does not generate a "buffer empty" interrupt, so the
    buffered implementation requires you to call the `update()` function!

By default the `Itm` class is non-buffered and therefore blocking.
You must set the `modm:platform:itm:buffer.tx` option to non-zero to use the
buffered version!


## Receiving Data

You must use OpenOCD to configure your target's hardware, as well as capture
the asynchronous trace stream.

!!! warning "Is the SWO pin connected to your debugger?"
    Check if your debugger supports trace output and if the SWO pin is actually
    connected to it on your development board!

To log the output to a file called `itm.fifo` you can call the `modm_log_itm`
command manually.

```sh
openocd -f modm/openocd.cfg -c "modm_log_itm itm.fifo 64000000"
```

You can then either use `tail -f itm.fifo` to display the raw data stream
(which is often enough) or use the [`itmdump` program](https://docs.rs/itm/)
for an advanced use-case (like de-multiplexing data streams).

!!! note "OpenOCD requires the CPU/HCLK frequency"
    You must provide the CPU/HCLK frequency of your target, so that
    OpenOCD can correctly set the prescaler for the asynchronous SWO output.

When using the `modm:build:scons` module, you can also use the convenience
wrapper, which logs to a temporary file and displays it's contents on the
terminal

```
 $ scons log-itm fcpu=64000000
╭───OpenOCD───> Single Wire Viewer
╰─────SWO────── stm32f103rbt
Hello from the SWO.
debug
info
warning
error
loop 0
loop 1
loop 2
loop 3
loop 4
loop 5
```


