// coding: utf-8
// ----------------------------------------------------------------------------

#include <xpcc/architecture/general/gpio.hpp>
#include <xpcc/architecture/general/time/delay.hpp>

using namespace xpcc;

// LEDs
CREATE_OUTPUT_PIN(LedGreen, B, 0);
CREATE_OUTPUT_PIN(LedRed, B, 1);

// ----------------------------------------------------------------------------
int
main(void) __attribute__((OS_main));

int
main()
{
	LedGreen::setOutput();
	LedGreen::reset();
	
	LedRed::setOutput();
	LedRed::set();
	
	while (1)
	{
		delay_ms(200);
		LedGreen::toggle();
	}
}
