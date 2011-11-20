
#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

GPIO__OUTPUT(LedStat, C, 12);		// inverted, 0=on, 1=off
GPIO__INPUT(ButtonWakeUp, A, 0);	// 1=pressed, 0=not pressed

extern "C" void
TIM3_IRQHandler(void)
{
	Timer3::acknowledgeInterrupt(Timer3::UPDATE_INTERRUPT);
	
	LedStat::toggle();
}

MAIN_FUNCTION
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	
	// The Button has an external Pull-Down resistor
	ButtonWakeUp::setInput(xpcc::stm32::INPUT, xpcc::stm32::FLOATING);

	Timer3::enable();
	Timer3::configureCounter(Timer3::ONE_SHOT);
	
	// enable Update Interrupt
	Timer3::enableInterrupt(Timer3::UPDATE_INTERRUPT);
	Timer3::setPeriod(1000000);	// 1s
	
	while (1)
	{
		if (ButtonWakeUp::read() == xpcc::gpio::HIGH)
		{
			// restart timer
			Timer3::refresh();
			Timer3::resume();

			LedStat::reset();
		}
	}
}

