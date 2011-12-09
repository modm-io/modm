
#include <xpcc/architecture.hpp>

static bool
initClock(){
	typedef xpcc::stm32::Core::Clock C;
	// use external 8MHz crystal, stm32f1
	if(!C::enableHSE(C::HSE_CRYSTAL))
		return false;
	C::enablePll(C::PLL_HSE, C::PLL_MUL_9);
	return C::switchToPll();
}

xpcc::stm32::Usart2 uart(115200);

MAIN_FUNCTION
{
	initClock();

	uart.write("Hello World!\n");
	
	while (1)
	{
	}
}

