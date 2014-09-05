/**
 * This example is to demonstrate the External Interrupt (EXTI)
 *
 * The red/green LEDs toggles by busy waiting.
 * When the button is pressed the blue LED flashes
 * for a very short moment in the interrupt handler.
 *
 */

#include <xpcc/architecture.hpp>
#include "../stm32f4_discovery.hpp"

typedef GpioInputE11 Irq;


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
	Irq::acknowledgeExternalInterruptFlag();
	LedBlue::set();
	xpcc::delayMicroseconds(1000);
	LedBlue::reset();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::Low);

	Button::setInput();
	Irq::setInput();

	Irq::setInputTrigger(Irq::InputTrigger::RisingEdge);
	Irq::enableExternalInterrupt();
	Irq::enableExternalInterruptVector(14);

	while (1)
	{
		LedRed::toggle();
		LedGreen::toggle();
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
