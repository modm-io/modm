# General Purpose I/O (GPIO)

This module provides register access to GPIO and connect their signals to the
respective peripherals in a compile-time verified way. This module also enables
all GPIO peripheral clocks on startup by default.

Each GPIO is represented as its own class with only static methods, which
implement the `modm::GpioIO` interface and provide additional platform-specific
methods.

```cpp
using namespace modm::platform;

using Button = GpioA0;
Button::setInput(Gpio::InputType::PullUp);
bool input = Button::read();

using Led = GpioInverted<GpioB3>; // inverts the IO logic of the pin

Led::setOutput(Gpio::OutputType::OpenDrain, Gpio::OutputSpeed::MHz2);
Led::set(input);

using Analog = GpioC12;
Analog::setAnalogInput(); // Use pin for ADC/DAC/COMP
Analog::lock(); // this prevents changes until next reboot

using Signal = GpioD9;
Signal::setAlternateFunction(4); // For AF id see datasheet
Signal::setAlternateFunction(); // STM32F1 has no AF id
Signal::disconnect(); // Switch back to floating input

// Some STM32s have remappable pinouts
GpioA11::remap(); // STM32G0: Remap A9 -> A11.
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
using Port = SoftwareGpioPort<GpioA10, GpioA2, GpioA6, GpioA14>;
static_assert(Port::number_of_ports == 1, "Read/write needs to be atomic");
Port::setOutput(Gpio::OutputType::OpenDrain);
Port::configure(Gpio::InputType::PullUp);
uint8_t nibble = Port::read();
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


## Alternate Function Signals

To make it easier to connect pins with peripherals, this module implements a
compile-time map of (pin, signal, peripheral) to Alternate Function ID (AF).
Note that you must provide both peripherals and signals to be unambiguous.

```cpp
GpioConnector<Peripheral::Usart1, GpioD0::Tx, GpioD1::Rx>::connect();
```

However, it is recommended to wrap this functionality into a separate function
`Driver::connect<Signals...>(config)`, so that additional driver specific pin
configuration can be done:

```cpp
template< class... Signals >
void Uart1::connect()
{
	Connector = GpioConnector<Peripheral::Usart1, Signals...>;
	Connector::disconnect(); // reset to floating input

	// extract pins from signals
	using Rx = Connector::GetSignal<Gpio::Signal::Rx>;
	using Tx = Connector::GetSignal<Gpio::Signal::Tx>;
	// if not found, returns GpioUnused, you can check for this case
	static_assert(not Connector::isValid<Tx>,
	              "This UART driver requires the Tx signal");

	// configure both pins
	Rx::configure(Gpio::InputType::PullUp);
	Tx::configure(Gpio::OutputType::PushPull);

	// connect both pins to alternate functions
	// This will static assert if signals do not make sense
	Connector::connect();
}
// Connect these pin signals to Usart1
Uart1::connect<GpioD0::Tx, GpioD1::Rx>();
```

Note that you may pass a *variable* number of signals to this connect function,
leaving out signals you don't need and adding signals that are not required.

```cpp
// Connect only one signal
Uart1::connect<GpioD0::Tx>();
// Connect more signals than required
Uart1::connect<GpioD0::Tx, GpioD2::Cts>();
```
