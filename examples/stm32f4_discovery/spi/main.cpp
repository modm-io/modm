#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"


MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	// Enable SPI 2
	GpioOutputB12::connect(SpiSimpleMaster2::Nss);
	GpioOutputB15::connect(SpiSimpleMaster2::Mosi);
	GpioInputB14::connect(SpiSimpleMaster2::Miso);
	GpioOutputB13::connect(SpiSimpleMaster2::Sck);
	SpiSimpleMaster2::initialize<defaultSystemClock, MHz20>();

	while (1)
	{
		SpiSimpleMaster1::transferBlocking(0xF0);
	}

	return 0;
}
