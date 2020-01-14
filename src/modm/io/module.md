# Input/Output Streams

The `modm::IOStream` class contains efficient formatting that supports both
C++ `std::basic_ostream`-like formatting via `operator <<` as well as
implementing a reasonably large subset of `printf`.

```cpp
modm::IOStream stream(device);
stream << "format number 8: " << uint8_t(8) << " or as signed -100: " << int8_t(-100);
stream << modm::endl;

stream.printf("format number 8: %u or as signed -100: %d", 8, -100);
```

!!! warning "AVR supported features"
    All expensive features incl. printf are disabled by default! Check the options.

!!! note "`modm::endl` does NOT implicitly flush!"
    Flushing is *extremely expensive* on embedded systems, therefore `modm::endl`
    does not implicitly flush the stream. Please call `modm::flush` explicitly.


## Using printf

This module uses the printf implementation from [`mpaland/printf`](https://github.com/mpaland/printf).

### Format Specifiers

A format specifier follows this prototype: `%[flags][width][.precision][length]type`
The following format specifiers are supported:


#### Supported Types

| Type   | Output |
|--------|--------|
| d or i | Signed decimal integer |
| u      | Unsigned decimal integer |
| b      | Unsigned binary |
| o      | Unsigned octal |
| x      | Unsigned hexadecimal integer (lowercase) |
| X      | Unsigned hexadecimal integer (uppercase) |
| f or F | Decimal floating point (`with_float` option) |
| e or E | Scientific-notation (exponential) floating point (`with_float` option) |
| g or G | Scientific or decimal floating point (`with_float` option) |
| c      | Single character |
| s      | String of characters |
| p      | Pointer address |
| %      | A % followed by another % character will write a single % |


#### Supported Flags

| Flags | Description |
|-------|-------------|
| -     | Left-justify within the given field width; Right justification is the default. |
| +     | Forces to precede the result with a plus or minus sign (+ or -) even for positive numbers.<br>By default, only negative numbers are preceded with a - sign. |
| (space) | If no sign is going to be written, a blank space is inserted before the value. |
| #     | Used with o, b, x or X specifiers the value is preceded with 0, 0b, 0x or 0X respectively for values different than zero.<br>Used with f, F it forces the written output to contain a decimal point even if no more digits follow. By default, if no digits follow, no decimal point is written. |
| 0     | Left-pads the number with zeros (0) instead of spaces when padding is specified (see width sub-specifier). |


#### Supported Width

| Width    | Description |
|----------|-------------|
| (number) | Minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is not truncated even if the result is larger. |
| *        | The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted. |


#### Supported Precision

| Precision | Description |
|-----------|-------------|
| .number   | For integer specifiers (d, i, o, u, x, X): precision specifies the minimum number of digits to be written. If the value to be written is shorter than this number, the result is padded with leading zeros. The value is not truncated even if the result is longer. A precision of 0 means that no character is written for the value 0.<br>For f and F specifiers: this is the number of digits to be printed after the decimal point. **By default, this is 6, maximum is 9**.<br>For s: this is the maximum number of characters to be printed. By default all characters are printed until the ending null character is encountered.<br>If the period is specified without an explicit value for precision, 0 is assumed. |
| .*        | The precision is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted. |


#### Supported Length

The length sub-specifier modifies the length of the data type.

| Length | d i  | u o x X |
|--------|------|---------|
| (none) | int  | unsigned int |
| hh     | char | unsigned char |
| h      | short int | unsigned short int |
| l      | long int | unsigned long int |
| ll     | long long int | unsigned long long int (`with_long_long` option) |
| j      | intmax_t | uintmax_t |
| z      | size_t | size_t |
| t      | ptrdiff_t | ptrdiff_t (`with_ptrdiff` option) |


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
