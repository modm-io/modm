# Standalone printf/sprintf formatted printing function library

This is a small but fully-loaded implementation of C's formatted printing family
of functions. It was originally designed by Marco Paland, with the primary use
case being in embedded systems - where these functions are unavailable, or when
one needs to avoid the memory footprint of linking against a full-fledged libc.
The library can be made even smaller by partially excluding some of the
supported format specifiers during compilation. The library stands alone, with
**no external dependencies**.

For the library API see https://github.com/eyalroz/printf#library-api.


## Configuration

On embedded targets, the `printf` family of functions are automatically replaced
by this module using the `-fno-builtin-printf` compile flag to prevent compiler
optimizations to substitute simple calls with `puts`.
64-bit integer support is disabled by default on AVRs.
You can overwrite these defaults in a `modm_config_printf.h` file placed into
your top-level include path:

```c
// 64-bit long long support is disabled on AVR by default
#define PRINTF_SUPPORT_LONG_LONG 1
// 5 instead of 6 for backwards compatibility
#define PRINTF_DEFAULT_FLOAT_PRECISION  5
// Only on embedded targets, on hosted the libc implementation is used
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES 1
```


## Redirecting the Output

All output is directed to a single function, which you must overwrite in your
application to the interface of your choice.

```cpp
extern "C" void putchar_(char c)
{
    // Redirect the printf output to UART.
    Board::stlink::Uart::write(c);
    // or IOStream if the board already has a logger
    // MODM_LOG_INFO << c;
}
```
