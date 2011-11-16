
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LedStat, C, 12);		// inverted, 0=on, 1=off

int
main(void)
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	
	while (1)
	{
		LedStat::toggle();
		xpcc::delay_ms(500);
	}
}

