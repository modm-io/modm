
#include <xpcc/architecture.hpp>

#include <xpcc/driver/can/mcp2515.hpp>
#include <xpcc/driver/software_spi.hpp>

GPIO__OUTPUT(Cs, B, 4);
GPIO__INPUT(Int, B, 2);

GPIO__OUTPUT(Sclk, B, 7);
GPIO__OUTPUT(Mosi, B, 5);
GPIO__INPUT(Miso, B, 6);

typedef xpcc::SoftwareSpi<Sclk, Mosi, Miso> Spi;

xpcc::Mcp2515<Spi, Cs, Int> mcp2515;

// Default filters to receive any extended CAN frame
FLASH(uint8_t canFilter[]) =
{
	MCP2515_FILTER_EXTENDED(0),	// Filter 0
	MCP2515_FILTER_EXTENDED(0),	// Filter 1

	MCP2515_FILTER_EXTENDED(0),	// Filter 2
	MCP2515_FILTER_EXTENDED(0),	// Filter 3
	MCP2515_FILTER_EXTENDED(0),	// Filter 4
	MCP2515_FILTER_EXTENDED(0),	// Filter 5

	MCP2515_FILTER_EXTENDED(0),	// Mask 0
	MCP2515_FILTER_EXTENDED(0),	// Mask 1
};

int
main()
{
	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	Spi::initialize();
	Cs::setOutput();
	Int::configure(xpcc::gpio::PULLUP);
	
	// Configure MCP2515 and set the filters
	mcp2515.initialize(xpcc::can::BITRATE_125_KBPS);
	mcp2515.setFilter(xpcc::accessor::asFlash(canFilter));
	
	// Create a new message
	xpcc::can::Message message(0x123456);
	message.length = 2;
	message.data[0] = 0xab;
	message.data[1] = 0xcd;
	
	mcp2515.sendMessage(message);
	
	while (1)
	{
		if (mcp2515.isMessageAvailable())
		{
			xpcc::can::Message message;
			if (mcp2515.getMessage(message))
			{
				// do something
			}
		}
	}
}
