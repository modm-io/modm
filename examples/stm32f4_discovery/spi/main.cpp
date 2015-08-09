#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"


MAIN_FUNCTION
{
	Board::initialize();

	// Enable SPI 2
	GpioOutputB12::connect(SpiMaster2::Nss);
	GpioOutputB15::connect(SpiMaster2::Mosi);
	GpioInputB14::connect(SpiMaster2::Miso);
	GpioOutputB13::connect(SpiMaster2::Sck);
	SpiMaster2::initialize<Board::systemClock, MHz20>();

	while (1)
	{
		SpiMaster1::transferBlocking(0xF0);
	}

	return 0;
}
