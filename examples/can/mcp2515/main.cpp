
#include <xpcc/architecture.hpp>

#include <xpcc/driver/can/mcp2515.hpp>
#include <xpcc/driver/software_spi.hpp>

namespace pin
{
	GPIO__OUTPUT(CS, B, 4);
	GPIO__INPUT(INT, B, 2);

	GPIO__OUTPUT(SCLK, B, 7);
	GPIO__OUTPUT(MOSI, B, 5);
	GPIO__INPUT(MISO, B, 6);
}

typedef xpcc::SoftwareSpi<pin::SCLK, pin::MOSI, pin::MISO> CanSpi;
xpcc::Mcp2515< CanSpi, pin::CS, pin::INT > mcp2515;

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
	CanSpi::initialize();
	pin::CS::setOutput();
	pin::INT::configure(xpcc::gpio::PULLUP);
	
	mcp2515.initialize(xpcc::can::BITRATE_125_KBPS);
	mcp2515.setFilter(xpcc::accessor::asFlash(canFilter));
	
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
