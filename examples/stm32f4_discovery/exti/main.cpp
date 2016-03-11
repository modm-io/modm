/**
 * This example is to demonstrate the External Interrupt (EXTI)
 *
 * The red/green LEDs toggles by busy waiting.
 * When the button is pressed the blue LED flashes
 * for a very short moment in the interrupt handler.
 *
 */

#include <xpcc/architecture/platform.hpp>

using namespace Board;

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
EXTI0_IRQHandler(void)
{
	Button::acknowledgeExternalInterruptFlag();
	LedBlue::set();
	xpcc::delayMicroseconds(1000);
	LedBlue::reset();
}


extern "C" void
EXTI15_10_IRQHandler(void)
{
	Irq::acknowledgeExternalInterruptFlag();
	LedOrange::set();
	xpcc::delayMicroseconds(1000);
	LedOrange::reset();
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// will be used to indicate external interrupts
	LedOrange::setOutput(xpcc::Gpio::Low);
	LedBlue::setOutput(xpcc::Gpio::Low);

	// will be toggled to indicate that the program is still running
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);

	// push the button to see the blue led light up
	Button::setInput(Gpio::InputType::Floating);
	Button::setInputTrigger(Button::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
	Button::enableExternalInterruptVector(14);

	// pull pin E11 low to see the orange led light up
	Irq::setInput(Gpio::InputType::PullUp);
	Irq::setInputTrigger(Irq::InputTrigger::BothEdges);
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
