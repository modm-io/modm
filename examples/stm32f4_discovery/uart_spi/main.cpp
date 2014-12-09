#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"


MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	// Enable Uart SPI 2
	GpioOutputA2::connect(UartSpiMaster2::Mosi);
	GpioInputA3::connect(UartSpiMaster2::Miso);
	GpioOutputA4::connect(UartSpiMaster2::Sck);
	UartSpiMaster2::initialize<defaultSystemClock, 5250000, xpcc::Tolerance::Exact>();

	while (1)
	{
		UartSpiMaster2::transferBlocking(0xF0);
	}

	return 0;
}
