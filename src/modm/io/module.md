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

## Using printf

The format string is composed of zero or more directives: ordinary
characters (not %), which are copied unchanged to the output stream;
and conversion specifications, each of which results in fetching zero
or more subsequent arguments. Each conversion specification is
introduced by the % character. The arguments must properly correspond
(after type promotion) with the conversion specifier. After the %,
the following appear in sequence:

Zero or more of the following flags:

- `-` A negative field width flag; the converted value is to be
  left adjusted on the field boundary. The converted value is
  padded on the right with blanks, rather than on the left with
  blanks.
- `' '` (space) A blank should be left before a positive number
  produced by a signed conversion (d).
- `+` A sign must always be placed before a number produced by a
  signed conversion. A `+` overrides a space if both are used.
- An optional decimal digit string specifying a minimum field width.
  If the converted value has fewer characters than the field width,
  it will be padded with spaces on the left (or right, if the
  left-adjustment flag has been given) to fill out the field width.
- An optional `h`, `l` or `ll` length modifier, that specifies that the argument
  for the `d`, `u`, or `x` conversion is a 8-bit ("h"), 32-bit ("l") or
  64-bit ("ll") rather than 16-bit.

The conversion specifiers and their meanings are:

- `c`: char (8-bit)
- `s`: string (`char *`)
- `p`: pointer (`void *`)
- `d`: signed  decimal
- `u`: unsigned decimal
- `x`: hex
- `f`: float
- `%`: %

Combined with the length modifiers you get:

- `d`:    signed 16-bit
- `ld`:   signed 32-bit
- `lld`:  signed 64-bit (not yet)
- `u`:    unsigned 16-bit
- `lu`:   unsigned 32-bit
- `llu`:  unsigned 64-bit (not yet)
- `hx`:    8-bit hex
- `x`:    16-bit hex
- `lx`:   32-bit hex
- `llx`:  64-bit hex (not yet)

Examples, given `-100` as argument in the right type:

```
%c:
%s:
%p:  0x0100
%d:  -100
%ld: -100
%u:  65436
%lu:
%hx: 0x9c
%x:  0xff9c
%lx: 0xffffff9c
```

- `s`: The "char *" argument is expected to be a pointer to an array
  of character type (pointer to a string). Characters from the array
  are written up to (but not including) a terminating NULL character;
  if a precision is specified, no more than the number specified are
  written. If a precision is given, no null character need be present;
  if the precision is not specified, or is greater than the size of
  the array, the array must contain a terminating NULL character.
- `%`: A `%` is written. No argument is converted. The complete conversion
  specification is `%%`.

In no case does a non-existent or small field width cause truncation
of a numeric field; if the result of a conversion is wider than the
field width, the field is expanded to contain the conversion result.


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