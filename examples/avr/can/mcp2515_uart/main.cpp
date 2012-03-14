
#include <xpcc/architecture.hpp>

#include <xpcc/driver/connectivity/can/mcp2515.hpp>
#include <xpcc/driver/connectivity/spi/software_spi.hpp>

#include <xpcc/workflow/periodic_timer.hpp>

GPIO__OUTPUT(LedGreen, B, 0);
GPIO__OUTPUT(LedRed, B, 1);

GPIO__OUTPUT(Cs, B, 4);
GPIO__INPUT(Int, B, 2);

GPIO__OUTPUT(Sclk, B, 7);
GPIO__OUTPUT(Mosi, B, 5);
GPIO__INPUT(Miso, B, 6);

typedef xpcc::SoftwareSpi<Sclk, Mosi, Miso> Spi;

xpcc::Mcp2515<Spi, Cs, Int> mcp2515;

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
xpcc::atmega::BufferedUart0 uart(115200);

// timer interrupt routine
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
}

int
main()
{
	LedGreen::setOutput(xpcc::gpio::HIGH);
	LedRed::setOutput(xpcc::gpio::LOW);
	
	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::BufferedUart0> device(uart);
	xpcc::IOStream stream(device);
	
	// enable interrupts
	sei();
	
	stream << "CAN Demo" << xpcc::endl;
	
	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	Spi::initialize();
	Cs::setOutput();
	Int::setInput(xpcc::atmega::PULLUP);
	
	// Configure MCP2515 and set the filters
	mcp2515.initialize(xpcc::can::BITRATE_125_KBPS);
	mcp2515.setFilter(xpcc::accessor::asFlash(canFilter));
	
	// Create a new message
	xpcc::can::Message message(0x123456);
	message.length = 2;
	message.data[0] = 0xab;
	message.data[1] = 0xcd;
	
	mcp2515.sendMessage(message);
	
	xpcc::PeriodicTimer<> timer(200);
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
		
		if (timer.isExpired())
		{
			LedGreen::toggle();
		}
	}
}
