
#include <xpcc/architecture.hpp>

#include <xpcc/driver/can/mcp2515.hpp>
#include <xpcc/processing/timer.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

typedef GpioOutputB0 LedGreen;
typedef GpioOutputB1 LedRed;

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

// Create a new UART object and configure it to a baudrate of 115200
Uart0 uart;

// timer interrupt routine
XPCC_ISR(TIMER2_COMPA)
{
	xpcc::Clock::increment();
}

int
main()
{
	LedGreen::setOutput(xpcc::Gpio::High);
	LedRed::setOutput(xpcc::Gpio::Low);

	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;

    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
    Uart0::initialize<clock, 115200>();

	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > device(uart);
	xpcc::IOStream stream(device);

	// enable interrupts
	sei();

	stream << "CAN Demo" << xpcc::endl;

	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	SPI::initialize<clock, 1000000>();
    Cs::setOutput();
	Int::setInput(Gpio::InputType::PullUp);

	// Configure MCP2515 and set the filters
    // Fixme: xpcc::Can::Bitrate is incompatitlbe with device driver
//	mcp2515.initialize(xpcc::can::BITRATE_125_KBPS);
	mcp2515.setFilter(xpcc::accessor::asFlash(canFilter));

	// Create a new message
	xpcc::can::Message message(0x123456);
	message.length = 2;
	message.data[0] = 0xab;
	message.data[1] = 0xcd;

	mcp2515.sendMessage(message);

	xpcc::ShortPeriodicTimer timer(200);
	while (1)
	{
		if (mcp2515.isMessageAvailable())
		{
			xpcc::can::Message message;
			if (mcp2515.getMessage(message))
			{
				LedRed::toggle();

				stream << "Message received:" << xpcc::endl;
				stream.printf(" id   = %lx", message.getIdentifier());
				if (message.isExtended()) {
					stream << " extended";
				}
				else {
					stream << " standard";
				}
				if (message.isRemoteTransmitRequest()) {
					stream << ", rtr";
				}
				stream << xpcc::endl;

				stream << " dlc  = " << message.getLength() << xpcc::endl;
				if (!message.isRemoteTransmitRequest())
				{
					stream << " data = ";
					for (uint32_t i = 0; i < message.getLength(); ++i) {
						stream << xpcc::hex << message.data[i] << xpcc::ascii << ' ';
					}
					stream << xpcc::endl;
				}
			}
		}

		if (timer.execute())
		{
			LedGreen::toggle();
		}
	}
}
