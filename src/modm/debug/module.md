# Logging

The `modm::log::Logger` uses a `modm::IOStream` to read messages and write them
to a given output device.

Write a log message with streams:

```cpp
MODM_LOG_DEBUG << "i=" << i << ", y=" << y << modm::flush;
```

The name of the stream represents the log level of the message. Available are:

- `MODM_LOG_DEBUG`
- `MODM_LOG_INFO`
- `MODM_LOG_WARNING`
- `MODM_LOG_ERROR`

A log message can also be generated separately:

```cpp
int a[3] = {1,2,3};
MODM_LOG_DEBUG << "a = ";
for(int i = 0; i < sizeof(a); ++i) {
	MODM_LOG_DEBUG << a[i] << ", ";
}
MODM_LOG_DEBUG << modm::flush;
```

### Flow of a call

This is to give an estimation how many resources a call of the logger use.
All considerations are for a PC program.

The given call is:

```cpp
MODM_LOG_DEBUG << 123456;
```

The macro resolves to:

```cpp
if (MODM_LOG_LEVEL <= modm::log::DEBUG)
    modm::log::debug
```

- `modm::log::debug` is an instance of `modm::Logger`:
   Call of `modm::Logger::operator << (T)` (with `T = int32_t`) which is inline
   - `IOStream::operator << (T)` (with `T = int32_t`) is inline
   - `IOStream::writeInteger(int32_t)`
- `IOStream::writeInteger(int32_t)` will create the formatted string
- virtual call of `IODevice::write(const char*)`
- redirect to `std::cout`

In sum there are two nested method calls with one of them being virtual.
