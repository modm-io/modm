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
`write()` and `read()` functions into an `IODevice`:

```cpp
// configure a UART
using Uart = Uart0;

// wrap it into an IODevice
modm::IODeviceWrapper<Uart, modm::IOBuffer::BlockIfFull> device;

// use this device to print a message
device.write("Hello");

// or create a IOStream and use the stream to print something
modm::IOStream stream(device);
stream << " World!";
```


## IODevice Buffer Behavior

The `modm::IODeviceWrapper` can be configured to discard or block in case the
device is full. Discarding data is always non-blocking, however, blocking on
write involves waiting in a loop until the device has space. This can cause a
deadlock when called inside an interrupt!

If compiled with the `modm:processing:fiber` module, the blocking behavior
allows the fiber to yield while waiting for the device. To prevent interlaced
output from different fibers, the `write(char)` function is protected by a
mutex that releases when a newline character `\n` is written. The `flush()`
function is also mutex protected to allow one fiber to reliably flush the
stream without having other fibers push data into the device.
