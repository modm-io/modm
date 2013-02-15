/**
 * This example is to demonstrate the External Interrupt (EXTI)
 *
 * The red/green LEDs toggle by busy waiting.
 * When the button is pressed the blue LED flashes
 * for a very short moment in the interrupt.
 *
 */

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

extern "C" void
EXTI0_IRQHandler(void)
{
	LedBlue::set();
	xpcc::delay_us(1000);
	LedBlue::reset();

	// Fourth, reset interrupt after it triggered
	EXTI->PR |= (1 << 0);
}

static ALWAYS_INLINE void
nvicEnableInterrupt(IRQn_Type IRQn)
{
	NVIC->ISER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

static bool
initClock()
{
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HseConfig::HSE_CRYSTAL)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, 4, 168);
	return Clock::switchToPll();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedOrange::setOutput(xpcc::gpio::HIGH);
	LedGreen::setOutput(xpcc::gpio::LOW);
	LedRed::setOutput(xpcc::gpio::HIGH);
	LedBlue::setOutput(xpcc::gpio::LOW);
	
	Button::setInput();

	/**
	 * Set EXTI:
	 * The button is connected to PA0 which is connected to EXTI0 Interrupt line.
	 */

	// First, connect PA0 line to the EXTI0 line
	SYSCFG->EXTICR[0] |= (1 << 0);

	// Second, set the interrupt mask and edge for EXTI0 line
	EXTI->IMR |= (1 << 0);
	EXTI->RTSR |= (1 << 0);

	// Third, set priority and enable EXTI0 interrupt
	NVIC_SetPriority(EXTI0_IRQn, 14);
	NVIC_EnableIRQ(EXTI0_IRQn);

	while (1)
	{
		LedRed::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}

	return 0;
}
