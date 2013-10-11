
#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"


MAIN_FUNCTION
{
	typedef SystemClock<Pll<ExternalClock<MHz8>, MHz168, MHz48> > systemClock;
	systemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::HIGH);
	LedGreen::setOutput(xpcc::Gpio::LOW);
	LedRed::setOutput(xpcc::Gpio::HIGH);
	LedBlue::setOutput(xpcc::Gpio::HIGH);

	// Enable SPI 2
	GpioOutputB12::connect(SpiSimpleMaster2::Nss);
	GpioOutputB15::connect(SpiSimpleMaster2::Mosi);
	GpioInputB14::connect(SpiSimpleMaster2::Miso);
	GpioOutputB13::connect(SpiSimpleMaster2::Sck);
	SpiSimpleMaster2::initialize<systemClock, MHz20>();

	while (1)
	{
		SpiSimpleMaster2::writeBlocking(0xF0);
	}

	return 0;
}
