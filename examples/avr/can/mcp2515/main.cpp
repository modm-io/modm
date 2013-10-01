
#include <xpcc/architecture.hpp>
#include <xpcc/driver/can/mcp2515.hpp>

using namespace xpcc::atmega;

typedef GpioOutputB4 Cs;
typedef GpioInputB2 Int;

typedef GpioOutputB7 Sclk;
typedef GpioOutputB5 Mosi;
typedef GpioInputB6 Miso;

typedef xpcc::SoftwareSpiMaster<Sclk, Mosi, Miso> SPI;

xpcc::Mcp2515<SPI, Cs, Int> mcp2515;

// Default filters to receive any extended CAN frame
FLASH_STORAGE(uint8_t canFilter[]) =
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
	SPI::initialize();
	Cs::setOutput();
	Int::setInput(Gpio::PullType::PullUp);

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
