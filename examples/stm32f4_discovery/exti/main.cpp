/**
 * This example is to demonstrate the External Interrupt (EXTI)
 *
 * The red/green LEDs toggles by busy waiting.
 * When the button is pressed the blue LED flashes
 * for a very short moment in the interrupt handler.
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
GPIO__INPUT(Irq, E, 11);

using namespace xpcc::stm32;

/* When you choose a different pin you must choose the corresponding
 * interrupt handler: (x in A, B, C, D, E, F, G, H, I)
 * Px0:          EXTI0_IRQHandler(void)
 * Px1:          EXTI1_IRQHandler(void)
 * Px2:          EXTI2_IRQHandler(void)
 * Px3:          EXTI3_IRQHandler(void)
 * Px4:          EXTI4_IRQHandler(void)
 * Px5  to Px9:  EXTI9_5_IRQHandler(void)
 * Px10 to Px15: EXTI15_10_IRQHandler(void)
 */
extern "C" void
//EXTI15_10_IRQHandler(void)
EXTI0_IRQHandler(void)
{
	ExtInt::resetInterruptFlag(ExtInt::Pin::PA0);

	LedBlue::set();
	xpcc::delay_us(1000);
	LedBlue::reset();
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
	Irq::setInput();

	ExtInt::enable(ExtInt::Pin::PA0);
	ExtInt::setMode(ExtInt::Pin::PA0, ExtInt::Mode::Rising);
	ExtInt::enableInterrupt(ExtInt::Pin::PA0);
	ExtInt::enableInterruptVector(ExtInt::Pin::PA0, 14);

	while (1)
	{
		LedRed::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}

	return 0;
}
