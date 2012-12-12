
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led, A, 8);
GPIO__INPUT(Button, C, 13);

using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Switch STM32F4 to 168 MHz (HSE clocked by an 25 MHz external clock)
	if (Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		Clock::enablePll(Clock::PllSource::PLL_HSE, 25, 336);
		Clock::switchToPll();
	}
	
	Led::setOutput();
	Button::setInput(xpcc::stm32::PULLUP);
	
	while (1) {
		Led::toggle();
		xpcc::delay_ms(500);
	}
}
