
#include <xpcc/architecture.hpp>
#include <xpcc/driver/ui/display/ea_dog.hpp>
#include <xpcc/driver/ui/display/font.hpp>
#include <xpcc/driver/ui/display/image.hpp>
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

using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
static void
displayMessage(const xpcc::can::Message& message)
{
	static uint32_t receiveCounter = 0;
	receiveCounter++;
	
	display.clear();
	display.setFont(xpcc::font::Assertion);
	display << "  CAN Demo";
	display.drawLine(0, 13, 102, 13);
	
	display.setCursor(0, 16);
	display.setFont(xpcc::font::FixedWidth5x8);
	
	display.printf("id  =%lx\n", message.getIdentifier());
	if (message.isExtended()) {
		display << " extended";
	}
	else {
		display << " standard";
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
			if (i == 4) {
				// wrap around to the next line
				display << xpcc::endl;
				display.setCursorX(x);
			}
			
			display << xpcc::hex << message.data[i] << xpcc::ascii << ' ';
		}
		display << xpcc::endl;
	}
	display << "# received=" << receiveCounter;
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
	display.drawImage(xpcc::glcd::Point(6, 0), xpcc::accessor::asFlash(bitmap::logo_xpcc_90x64));
	display.update();
	
	// Remap the Pins of CAN1 to PB8 and PB9.
	// This has to be done before calling initialize()!
	Can1::configurePins(Can1::REMAP_PB8_PB9);
	Can1::initialize(xpcc::can::BITRATE_125_KBPS);
	
	// Set filters
	CanFilter::setFilter(0, CanFilter::FIFO0, 
			CanFilter::ExtendedIdentifier(0x12345678),
			CanFilter::ExtendedFilterMask(0x1fffffff));
	CanFilter::setFilter(1, CanFilter::FIFO0, 
			CanFilter::ExtendedIdentifier(0x17000000),
			CanFilter::ExtendedFilterMask(0x1f000000));
	CanFilter::setFilter(2, CanFilter::FIFO1,
			CanFilter::StandardIdentifier(0x123),
			CanFilter::StandardFilterMask(0x3ff));
	CanFilter::setFilter(3, CanFilter::FIFO1,
			CanFilter::StandardIdentifier(0x321, CanFilter::RTR),
			CanFilter::StandardFilterMask(0x3ff));
	
	CanFilter::setIdentifierFilter(4, CanFilter::FIFO1,
			CanFilter::StandardIdentifier(0x111),
			CanFilter::ExtendedIdentifier(0x11111111));
	
	CanFilter::setFilterShort(5, CanFilter::FIFO1,
			CanFilter::StandardIdentifierShort(0x202),
			CanFilter::StandardFilterMaskShort(0x30f, CanFilter::RTR_DONT_CARE),
			CanFilter::ExtendedIdentifierShort(0x12000000),
			CanFilter::ExtendedFilterMaskShort(0x1f000000));
	CanFilter::setIdentifierFilterShort(6, CanFilter::FIFO1,
			CanFilter::StandardIdentifierShort(0x112),
			CanFilter::StandardIdentifierShort(0x113),
			CanFilter::StandardIdentifierShort(0x114, CanFilter::RTR),
			CanFilter::ExtendedIdentifierShort(0x11500000));
	
	// Send a 11-bit message
	xpcc::can::Message msg1(1, 0);
	msg1.setExtended(false);
	
	Can1::sendMessage(msg1);
	
	// Send a batch of 16 29-bit messages. Without setting
	// STM32_CAN1_TX_BUFFER_SIZE to at least 13 this operation will fail because
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
