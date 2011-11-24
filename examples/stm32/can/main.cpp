
#include <xpcc/architecture.hpp>
#include <xpcc/driver/ui/display/ea_dog.hpp>
#include <xpcc/driver/ui/display/font.hpp>
#include <xpcc/debug.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led1, A, 1);
GPIO__OUTPUT(Led2, A, 8);

GPIO__OUTPUT(LedStatInverted, C, 12);	// inverted, 0=on, 1=off
typedef xpcc::gpio::Invert<LedStatInverted> LedStat;

namespace lcd
{
	GPIO__OUTPUT(CS, C, 1);
	GPIO__OUTPUT(A0, C, 3);
	GPIO__OUTPUT(Reset, B, 5);
}

// Graphic LCD
xpcc::DogS102< xpcc::stm32::Spi1, lcd::CS, lcd::A0, lcd::Reset, false > display;

static uint32_t receiveCounter = 0;

using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
static void
displayMessage(const xpcc::can::Message& message)
{
	receiveCounter++;
	
	display.clear();
	display.setFont(xpcc::font::Assertion);
	display << "  CAN Demo";
	display.drawLine(0, 13, 102, 13);
	
	display.setCursor(0, 16);
	display.setFont(xpcc::font::FixedWidth5x8);
	
	display.printf("id  =%lx\n", message.getIdentifier());
	if (message.isExtended()) {
		display << "extended";
	}
	else {
		display << "standard";
	}
	if (message.isRemoteTransmitRequest()) {
		display << ", rtr";
	}
	display << xpcc::endl;
	
	display << "dlc =" << message.getLength() << xpcc::endl;
	if (!message.isRemoteTransmitRequest())
	{
		display << "data=";
		uint8_t x = display.getCursor().getX();
		for (uint32_t i = 0; i < message.getLength(); ++i) {
			display << xpcc::hex << message.data[i] << xpcc::ascii << ' ';
			
			if (i == 3) {
				display << xpcc::endl;
				display.setCursorX(x);
			}
		}
		display << xpcc::endl;
	}
	display << "#msg=" << receiveCounter;
	display.update();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	LedStatInverted::setOutput();
	LedStat::set();
	
	// Initialize display
	display.initialize();
	
	display.clear();
	display.setFont(xpcc::font::Assertion);
	display << "  CAN Demo";
	display.drawLine(0, 13, 102, 13);
	display.update();
	
	// Remap the Pins of CAN1 to PB8 and PB9.
	// This has to be done before calling initialize()!
	Can1::remapPins(Can1::REMAP_PB8_PB9);
	Can1::initialize(xpcc::can::BITRATE_125_KBPS);
	
	// Set filters
	Can1::setMaskFilter(0, Can1::FIFO0, 
			Can1::extendedFilter(0x12345678, Can1::EXTENDED, Can1::NO_RTR),
			Can1::extendedFilter(0x1fffffff, Can1::EXTENDED, Can1::RTR));
	Can1::setMaskFilter(1, Can1::FIFO0, 
			Can1::extendedFilter(0x17000000, Can1::EXTENDED, Can1::NO_RTR),
			Can1::extendedFilter(0x1f000000, Can1::EXTENDED, Can1::RTR));
	Can1::setMaskFilter(2, Can1::FIFO1,
			Can1::standardFilter(0x123, Can1::STANDARD, Can1::NO_RTR),
			Can1::standardFilter(0x3ff, Can1::EXTENDED, Can1::RTR));
	Can1::setMaskFilter(3, Can1::FIFO1,
			Can1::standardFilter(0x321, Can1::STANDARD, Can1::RTR),
			Can1::standardFilter(0x3ff, Can1::EXTENDED, Can1::RTR));
	
	Can1::setIdentifierFilter(4, Can1::FIFO1,
			Can1::standardFilter(0x111, Can1::STANDARD, Can1::NO_RTR),
			Can1::extendedFilter(0x11111111, Can1::EXTENDED, Can1::RTR));
	
	Can1::setSmallMaskFilter(5, Can1::FIFO1,
			Can1::standardSmallFilter(0x202, Can1::STANDARD, Can1::NO_RTR),
			Can1::standardSmallFilter(0x30f, Can1::STANDARD, Can1::NO_RTR),
			Can1::extendedSmallFilter(0x12000000, Can1::EXTENDED, Can1::NO_RTR),
			Can1::extendedSmallFilter(0x1f000000, Can1::EXTENDED, Can1::RTR));
	Can1::setSmallIdentifierFilter(6, Can1::FIFO1,
			Can1::standardSmallFilter(0x112, Can1::STANDARD, Can1::NO_RTR),
			Can1::standardSmallFilter(0x113, Can1::STANDARD, Can1::NO_RTR),
			Can1::standardSmallFilter(0x114, Can1::EXTENDED, Can1::NO_RTR),
			Can1::extendedSmallFilter(0x11500000, Can1::EXTENDED, Can1::NO_RTR));
	
	// Send a 11-bit message
	xpcc::can::Message msg1(1, 0);
	msg1.setExtended(false);
	
	Can1::sendMessage(msg1);
	
	// Send a batch of 16 29-bit messages. Without setting
	// STM32_CAN_TX_BUFFER_SIZE to at least 13 this operation will fail because
	// the STM32 has only three hardware transmission buffers => Led2 will
	// be off.
	xpcc::can::Message msg2(0x12345678, 1);
	for (uint32_t i = 0; i < 16; ++i) {
		msg2.data[0] = i;
		if (!Can1::sendMessage(msg2)) {
			LedStat::reset();
		}
	}
	
	while (1)
	{
		// Wait for a new message
		if (Can1::isMessageAvailable())
		{
			xpcc::can::Message message;
			Can1::getMessage(message);
			
			// Display the message on the DOGS102 display
			displayMessage(message);
		}
	}
}
