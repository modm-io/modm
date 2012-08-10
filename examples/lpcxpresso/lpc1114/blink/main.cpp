
#include <xpcc/architecture.hpp>

#define LED_TOGGLE_TICKS 150		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

GPIO__OUTPUT(Led, 0, 7);

int
main(void)
{
	SystemInit();
	
	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	// Set LED port pin to output
	Led::setOutput(false);

	// check delay_ms timing
	for (uint8_t ii = 10; ii > 0; --ii)
	{
		for (uint8_t jj = 4; jj > 0; --jj)
		{
			Led::toggle();
			xpcc::delay_ms(100);
		}
		xpcc::delay_ms(600);
	}
	
	while (1)
	{
		// Each time we wake up...
		// Check TimeTick to see whether to set or clear the LED I/O pin
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX)) < ((LED_TOGGLE_TICKS / COUNT_MAX) / 2)) {
			Led::reset();
		}
		else {
			Led::set();
		}
		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}
