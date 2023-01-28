# Input/Output Streams

The `modm::IOStream` class contains efficient formatting that supports both
C++ `std::basic_ostream`-like formatting via `operator <<` as well as
implementing `printf` via the `modm:printf` module.

```cpp
modm::IOStream stream(device);
stream << "format number 8: " << uint8_t(8) << " or as signed -100: " << int8_t(-100);
stream << modm::endl;

stream.printf("format number 8: %u or as signed -100: %d", 8, -100);
```

!!! warning "AVR supported features"
    All expensive features including `printf` are disabled by default to reduce
    firmware size! Please check the options.

!!! note "`modm::endl` does NOT implicitly flush!"
    Flushing is *extremely expensive* on embedded systems, therefore `modm::endl`
    does not implicitly flush the stream. Please call `modm::flush` explicitly.


## Redirecting IOStreams

The `modm::IODeviceWrapper` transforms any peripheral device that provides static
`write()` and `read()` functions into an `IODevice`.

You have to decide what happens when the device buffer is full and you cannot
write to it at the moment. There are two options:

1. busy wait until the buffer is free, or
2. discard the bytes that cannot be written.

Option 1 has the advantage, that none of your data will be lost,
however, busy-waiting can take a long time and can mess up your
program timings.
There is also a **high risk of deadlock**, when writing to a
IODevice inside of an interrupt and then busy-waiting forever
because the IODevice requires interrupts itself to send out
the data.

It is therefore highly recommended to use option 2, where surplus
data will be discarded.
You should increase the IODevice buffer size, if you experience
missing data from your connection.
This behavior is also deadlock safe when called from inside another
interrupt, and your program timing is minimally affected (essentially
only coping data into the buffer).

There is no default template argument, so that you hopefully make
a conscious decision and be aware of this behavior.

Example:

```cpp
// configure a UART
using Uart = Uart0;

// wrap it into an IODevice
modm::IODeviceWrapper<Uart, modm::IOBuffer::DiscardIfFull> device;

// use this device to print a message
device.write("Hello");

// or create a IOStream and use the stream to print something
modm::IOStream stream(device);
stream << " World!";
```
