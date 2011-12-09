
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LedStat, C, 12);		// inverted, 0=on, 1=off

static bool
initClock(){
	typedef xpcc::stm32::Core::Clock C;
	// use external 8MHz crystal, stm32f1
	if(!C::enableHSE(C::HSE_CRYSTAL))
		return false;
	C::enablePll(C::PLL_HSE, C::PLL_MUL_9);
	return C::switchToPll();
}

int
main(void)
{
	initClock();

	LedStat::setOutput(xpcc::gpio::HIGH);
	
	while (1)
	{
		LedStat::toggle();
		xpcc::delay_ms(500);
	}
}

