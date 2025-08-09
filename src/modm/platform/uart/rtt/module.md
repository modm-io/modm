# UART over RTT

The RTT channels are exposed as a UART interface which you can use like so:

```cpp
// Access data via static methods
using Rtt = modm::platform::Rtt<0>;
// Create a software loopback
if (uint8_t data; Rtt::read(data)) { Rtt::write(data); }

// Wrap into an IOStream
modm::IODeviceWrapper<Rtt, modm::IOBuffer::BlockIfFull> rtt_device;
modm::IOStream stream(rtt_device);
// Writing data
stream << "Hello World" << modm::endl;

// Reading data from an IOStream
if (char data; stream.get(data), data != modm::IOStream::eof)
{ /* process new data */ }
```

To configure the number and size of RTT channels, see the `modm:rtt` module.
