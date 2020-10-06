# SAM Signals API

This module provides a way to use the GPIO pins of the SAM device. The raw
GPIO API is defined with the following goals:

- Zero-cost compared to assembly
- Statically type checked
- Aligned (to the extent possible) with other modm platforms

Additionally, it also provides the Connection API which sets up the wiring
between GPIO pins and peripherals. Given how flexible SAM is, it is a challenge
to make this completely statically type checked.

The Connection API was designed with the following goals:

- Compilaton errors are understandable (contain necessary information about the context that lead to the error).
- Completely in C++ templates, allows peripherals to perform further validation.
- Allows the user to select the intended peripheral signal (e.g. TX, SCK, MOSI, SDA).
- Deduces the pin function at compile time
- Deduces the Peripheral pad at compile time (e.g. Sercom Pad)

As a few examples, the following situations will crash compilation:

- Usage of drivers which don't exist on the pin
- Passing a pin which has a connection to the driver but not the correct instance
- Passing a pin which connects to the driver/instance but not the correct Peripheral signal
- Using multiple signals from the same pin in a *single* Peripheral
- Using the same signal from multiple pins in a *single* Peripheral

!!! warning "The Connection API cannot check whether signals between multiple Peripherals overlap or not.
    This will be the case if you e.g. try to instantiate Sercom0 as both UART and SPI bus, or use a single
    pin for multiple different peripherals at the same time."

## Usage examples

### Toggle the A01 pin

```cpp
Gpio<A01>::toggle();
```

This compiles to the following on SAMD21 (4 clock cycles):

```asm
2232:	2580      	movs	r5, #128	; 0x80
2238:	4c03      	ldr	r4, [pc, #12]	; (2248 <main+0x18>)
223a:	02ad      	lsls	r5, r5, #10
223e:	6025      	str	r5, [r4, #0]
2248:	41005200 	.word	0x41005200
```

### Toggle the A01 and A02 pins together

```cpp
GpioSet<A01, A02>::toggle();
```

The assembly will look similar to the single pin example above. If you look carefully, you will
find that `Gpio<A01>` is actually using the `toggle()` method from `GpioSet<A01>`!

### Connect A01 pin to the UART1 TX pin

!!! note "Should normally be done by the UART peripheral!"

```cpp
using Sercom = Peripherals::Sercom<1>;
using Conn = Gpio<A01>::Tx::Connector<Sercom, Sercom::Pad<0>, Sercom::Pad<2>>;
Conn::::connect();
```

Validate the signal name used for the connection:

```cpp
static_assert(std::is_same_v<Sercom::Pad<2>, Conn::Signal>>, "wrong!");
```

