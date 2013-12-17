
#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

GPIO__OUTPUT(LedStat, C, 12);	// inverted, 0=on, 1=off

static bool
initClock()
{
	typedef xpcc::stm32::Clock C;
	
	// use external 8MHz crystal, stm32f1
	if (!C::enableHse(C::HseConfig::HSE_CRYSTAL)) {
		return false;
	}
	
	C::enablePll(C::PllSource::PLL_HSE, C::PllMul::PLL_MUL_9);
	return C::switchToPll();
}

extern "C" void
TIM3_IRQHandler(void)
{
	Timer3::resetInterruptFlags(Timer3::FLAG_UPDATE);
	
	LedStat::toggle();
}

MAIN_FUNCTION
{
	initClock();

	LedStat::setOutput(xpcc::Gpio::High);
	
	Timer3::enable();
	Timer3::setMode(Timer3::UP_COUNTER);
	
	// enable Update Interrupt
	Timer3::enableInterruptVector(true, 15);
	Timer3::enableInterrupt(Timer3::INTERRUPT_UPDATE);
	Timer3::setPeriod(200000);	// 200 ms
	
	// enable timer
	Timer3::start();

	while (1)
	{
	}
}

