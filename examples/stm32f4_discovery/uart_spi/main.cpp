#include "../stm32f4_discovery.hpp"


int
main()
{
	Board::initialize();

	// Enable Uart SPI 2
	GpioOutputA2::connect(UartSpiMaster2::Mosi);
	GpioInputA3::connect(UartSpiMaster2::Miso);
	GpioOutputA4::connect(UartSpiMaster2::Sck);
	UartSpiMaster2::initialize<Board::systemClock, 5250000, xpcc::Tolerance::Exact>();

	while (1)
	{
		UartSpiMaster2::transferBlocking(0xF0);
	}

	return 0;
}
