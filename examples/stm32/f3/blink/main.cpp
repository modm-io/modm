#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedOrange, D, 13);		// User LED 3
GPIO__OUTPUT(LedGreen, D, 12);		// User LED 4
GPIO__OUTPUT(LedRed, D, 14);		// User LED 5
GPIO__OUTPUT(LedBlue, D, 15);		// User LED 6

GPIO__OUTPUT(VBusPresent, A, 9);		// green LED (LD7)
GPIO__OUTPUT(VBusOvercurrent, D, 5);	// red LED   (LD8)

GPIO__INPUT(Button, A, 0);

using namespace xpcc::stm32;

static bool
initClock()
{
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, Clock::PllMul::MUL_8);
	return Clock::switchToPll();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);
	
	// Remap the Pins of CAN1 to PB8 and PB9.
	// This has to be done before calling initialize()!
	Can1::configurePins(Can1::REMAP_PB8_PB9);
	Can1::initialize(xpcc::can::BITRATE_125_KBPS, 9);

	Usart2 uart(115200);
	// STM32F3 specific mapping
	uart.configurePins(Usart2::Mapping::REMAP_PB3_PB4);
	uart.write('k');

	// COMP1 compile test
	Comp1::initialize(Comp1::InvertingInput::VREF);

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}

	return 0;
}
