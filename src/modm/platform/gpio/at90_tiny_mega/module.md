# General Purpose I/O (GPIO)

This module provides register access to GPIO and connect their signals to the
respective peripherals in a compile-time verified way.

Each GPIO is represented as its own class with only static methods, which
implement the `modm::GpioIO` interface and provide additional platform-specific
methods.

```cpp
using namespace modm::platform;

using Button = GpioA0;
Button::setInput(Gpio::InputType::PullUp);
bool input = Button::read();

using Led = GpioInverted<GpioB3>; // inverts the IO logic of the pin

Led::setOutput();
Led::set(input);
```

You can also use an unordered set of GPIOs, which is useful when configuring a
large number of pins, since the register accesses will be bundled and thus less
code is generated.

```cpp
using Set = GpioSet<GpioA0, GpioB1, GpioC2, GpioD3>;
Set::setInput();
```

To write and read a set of GPIOs, you need to use an ordered implementation,
which defines the pins from MSB to LSB, left-to-right. You can also check the
number of ports in case your use-case requires atomic reads/writes.

```cpp
using Port = SoftwareGpioPort<GpioA7, GpioA2, GpioA6, GpioA0>;
static_assert(Port::number_of_ports == 1, "Read/write needs to be atomic");
Port::setInput(Gpio::InputType::PullUp);
uint8_t nibble = Port::read();
Port::setOutput();
Port::write(nibble);
```

For efficient access you can use a strictly-ordered implementation with a start
pin and width. Note that you can reverse the data order with a negative width.

```cpp
using Port = GpioPort<GpioA0, 8>;
Port::setOutput();
Port::write(data);

using ReversePort = GpioPort<GpioB7, -8>;
ReversePort::setInput();
uint8_t data = ReversePort::read();
```

Finally, you can use an empty GPIO implementation in cases where the API
requires a GPIO, but you don't need one, for example, a bit-banged SPI without
MISO pin:

```cpp
// write only SPI
using SPI = modm::platform::BitBangSpiMaster<GpioA0, GpioA1, GpioUnused>;
```


## Peripheral Signals

To make it easier to connect pins with peripherals, this module implements a
compile-time map of (pin, signal, peripheral). Note that you must provide both
peripherals and signals to be unambiguous.

```cpp
GpioConnector<Peripheral::Uart0, GpioD0::Txd, GpioD1::Rxd>::connect();
```

However, it is recommended to wrap this functionality into a separate function
`Driver::connect<Signals...>(config)`, so that additional driver specific pin
configuration can be done:

```cpp
template< class... Signals >
void Uart0::connect()
{
	Connector = GpioConnector<Peripheral::Uart0, Signals...>;
	Connector::disconnect(); // reset to floating input

	// extract pins from signals
	using Rxd = Connector::GetSignal<Gpio::Signal::Rxd>;
	using Txd = Connector::GetSignal<Gpio::Signal::Txd>;
	// if not found, returns GpioUnused, you can check for this case
	static_assert(not Connector::isValid<Txd>,
	              "This UART driver requires the Txd signal");

	// configure both pins
	Rxd::configure(Gpio::InputType::PullUp);
	Txd::setOutput();

	// connect both pins to alternate functions
	// This will static assert if signals do not make sense
	Connector::connect();
}
// Connect these pin signals to Uart0
Uart0::connect<GpioD0::Txd, GpioD1::Rxd>();
```

Note that you may pass a *variable* number of signals to this connect function,
leaving out signals you don't need and adding signals that are not required.

```cpp
// Connect only one signal
Uart1::connect<GpioD0::Txd>();
// Connect more signals than required
Uart1::connect<GpioD0::Txd, GpioD2::Xck>();
```


## External Interrupts

You can also configure the external interrupts, however, you must
provide the interrupt yourself.

```cpp
GpioD2::setInputTrigger(Gpio::InputTrigger::RisingEdge);
GpioD2::enableExternalInterrupt();

MODM_ISR(INT0)
{
	// your code
	GpioD2::acknowledgeInterruptFlag();
}

GpioD2::disableExternalInterrupt();
```

When using multiple Pin-Change interrupts you need to first check the
corresponding flag to determine which pin triggered the collective interrupt.

```cpp
GpioB1::enablePcInterrupt();
GpioB2::enablePcInterrupt();

MODM_ISR(PCINT0)
{
	if (GpioB1::getPcInterruptFlag()) {
		bool state = GpioB2::read();
		// your code
		GpioB1::acknowledgePcInterruptFlag();
	}
	if (GpioB2::getPcInterruptFlag()) {
		bool state = GpioB2::read();
		// your code
		GpioB2::acknowledgePcInterruptFlag();
	}
}
```
