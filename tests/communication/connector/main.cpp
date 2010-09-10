
#include <xpcc/architecture.hpp>

#include <xpcc/driver/can/mcp2515.hpp>
#include <xpcc/driver/software_spi.hpp>

#include <xpcc/communication/backend/can/can_connector.hpp>

namespace pin
{
	GPIO__OUTPUT(CS, C, 4);
	GPIO__INPUT(INT, B, 0);

	GPIO__OUTPUT(SCLK, C, 5);
	GPIO__OUTPUT(MOSI, C, 7);
	GPIO__INPUT(MISO, C, 6);
}

typedef xpcc::SoftwareSpi<pin::SCLK, pin::MOSI, pin::MISO> CanSpi;
typedef xpcc::Mcp2515< CanSpi, pin::CS, pin::INT > CanDriver;

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

CanDriver mcp2515;

xpcc::CanConnector<CanDriver> connector(&mcp2515);

int
main()
{
	CanSpi::initialize();
	pin::CS::setOutput();
	pin::INT::configure(xpcc::gpio::PULLUP);
	
	mcp2515.initialize(xpcc::can::BITRATE_125_KBPS);
	mcp2515.setFilter(xpcc::accessor::asFlash(canFilter));
	
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
