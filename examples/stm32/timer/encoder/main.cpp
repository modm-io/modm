
#include <xpcc/architecture.hpp>
#include <xpcc/driver/ui/display/ea_dog.hpp>
#include <xpcc/driver/ui/display/font.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedStat, C, 12);	// inverted, 0=on, 1=off

namespace lcd
{
	GPIO__OUTPUT(CS, C, 1);
	GPIO__OUTPUT(A0, C, 3);
	GPIO__OUTPUT(Reset, B, 5);
}

// Graphic LCD
xpcc::DogS102< xpcc::stm32::Spi1, lcd::CS, lcd::A0, lcd::Reset, false > display;

GPIO__INPUT(EncoderIndex, C, 13);
GPIO__INPUT(EncoderA, C, 6);
GPIO__INPUT(EncoderB, C, 7);

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

MAIN_FUNCTION
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	
	EncoderIndex::setInput(xpcc::stm32::PULLUP);
	EncoderA::setInput(xpcc::stm32::PULLUP);
	EncoderB::setInput(xpcc::stm32::PULLUP);
	
	Timer3::enable();
	Timer3::remapPins(Timer3::FULL_REMAP);		// remap CH1 to C6 and CH2 to C7
	Timer3::setMode(Timer3::ENCODER);
	Timer3::setOverflow(24*4 - 1);
	Timer3::start();
	
	display.initialize();
	
	while (1)
	{
		const xpcc::glcd::Point center(70, 32);
		const uint16_t radius = 20;
		
		if (!EncoderIndex::read()) {
			Timer3::setValue(0);
		}

		// round the value to 0..23
		uint16_t index = ((Timer3::getValue() + 2) % (24*4)) / 4;

		display.clear();
		
		display.setCursor(4, 18);
		display.setFont(xpcc::font::Numbers14x32);
		display << index;
		
		display.drawCircle(center, radius);
		float angle = (index * 2 * M_PI) / 24;
		display.drawLine(center,
				center + xpcc::glcd::Point(
						 sin(angle) * radius,
						-cos(angle) * radius));
		display.update();
		
		xpcc::delay_ms(10);
	}
}

