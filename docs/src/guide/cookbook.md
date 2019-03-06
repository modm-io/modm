# Cookbook

Here are a few snippets of code we use frequently.
We assume `using namespace modm::platform;` to make the code more readable.

All modm code is either placed in the `modm` namespace or prefixed with
`modm_` or `MODM_` to avoid clashes with other code.
The platform specific code is placed in the `modm::platform` namespace and
is the only place that contains target specific code.

!!! tip "Check the examples"
    Have a look at the [`modm/examples/` folder][examples] for more advanced
    use cases.


## GPIO

```cpp
using Led = GpioOutputB0;
Led::setOutput();
Led::set();    // 1 instruction on AVR
Led::reset();  // 3 instructions on Cortex-M
Led::toggle();

using Button = GpioInputB0;
Button::setInput(Gpio::InputType::PullUp);
bool state = Button::read();
```


## Buffered UART

```cpp
using Uart = Uart0;
// connect both pins with a pullup on the Rx
Uart::connect<GpioOutputD1::Tx, GpioInputD0::Rx>(Gpio::InputType::PullUp);
// initialize to 115.2kBaud from the BSP clock configuration
Uart::initialize<Board::SystemClock, 115200_Bd>();

Uart::write('H');  // Ohai there
Uart::write('i');

uint8_t buffer;
while(1) {
    // create a simple loopback
    if (Uart::read(buffer)) {
        Uart::write(buffer);
    }
}
```


## IOStream

```cpp
using Uart = Uart0;
// Create a IODevice with the Uart
modm::IODeviceWrapper<Uart> device;
modm::IOStream stream(device);

Uart::connect<GpioOutputD1::Tx>();
Uart::initialize<Board::SystemClock, 115200_Bd>();

// similar to std::ostream but without formatting features
stream << 42 << " is a nice number!" << modm::endl;
// printf has the most common formatting features
stream.printf("%d is a %s number!\n", 42, "nice");
```


## Software Timers

```cpp
using Led = GpioOutputB0;
modm::Timeout timeout(10000);   // 10s timeout
modm::PeriodicTimer timer(250); // 250ms period

Led::setOutput(modm::Gpio::High);

while(1) {
    if (timeout.execute()) {
        // executes just once
        timer.stop();
        Led::reset();
    }
    if (timer.execute()) {
        // executes once per period
        Led::toggle();
    }
}
```

[examples]: https://github.com/modm-io/modm/tree/develop/examples