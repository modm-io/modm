
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LedGreenPin, B, 5);
GPIO__OUTPUT(LedYellowPin, B, 6);
GPIO__OUTPUT(LedRedPin, B, 7);

typedef xpcc::gpio::Invert<LedGreenPin> LedGreen;
typedef xpcc::gpio::Invert<LedYellowPin> LedYellow;
typedef xpcc::gpio::Invert<LedRedPin> LedRed;

void
init()
{
	LedGreen::reset();
	LedGreen::setOutput();
	LedYellow::reset();
	LedYellow::setOutput();
	LedRed::reset();
	LedRed::setOutput();
}

FLASH(uint8_t bla1[30000]) = {};
FLASH(uint8_t bla2[30000]) = {};
FLASH(uint8_t bla3[30000]) = {};

MAIN_FUNCTION
{
	init();
	
	volatile uint8_t a = bla1[0];
	volatile uint8_t b = bla2[0];
	volatile uint8_t c = bla3[0];
	
	while (1)
	{
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}
}
