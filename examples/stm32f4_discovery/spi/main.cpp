#include <xpcc/architecture/platform.hpp>

// If you use a different SPI instance, you may have to also choose different
// GPIOs to connect to.
using Sck = GpioOutputB13;
using Mosi = GpioOutputB15;
using Miso = GpioInputB14;
using SpiMaster = SpiMaster2;
// Note that you can also use a bit-banged SPI driver as a drop-in replacement
// using SpiMaster = xpcc::SoftwareSpiMaster<Sck, Mosi, Miso>;

int
main()
{
	Board::initialize();

	// Connect the GPIOs to the SPIs alternate function
	Sck::connect(SpiMaster::Sck);
	Mosi::connect(SpiMaster::Mosi);
	Miso::connect(SpiMaster::Miso);
	// Initialize the SPI with a 20MHz clock
	SpiMaster::initialize<Board::systemClock, MHz20>();

	uint8_t data = 0;
	while (1)
	{
		// Connect Mosi to Miso to create a loopback
		data = SpiMaster::transferBlocking(data);
		data++;
		// This will be spamming the bus
	}

	return 0;
}
