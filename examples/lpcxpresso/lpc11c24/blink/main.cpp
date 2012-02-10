
#include <xpcc/architecture.hpp>

#define LED_TOGGLE_TICKS 200		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

/*
static const uint8_t segmentlut[10] = {
//    FCPBAGED
	0b11011011, // 0
	0b01010000, // 1
	0b00011111, // 2
	0b01011101, // 3
	0b11010100, // 4
	0b11001101, // 5
	0b11001111, // 6
	0b01011000, // 7
	0b11011111, // 8
	0b11011101, // 9
};

void
SetSegment(int n)
{
	int i;
	
	GPIOSetValue(1, 11, 0);
	
	if (n < 0)
		n = 0;
	else
		n = segmentlut[n];
	
	for (i = 0; i < 8; i++) {
		if ((n >> (7 - i)) & 1)
			GPIOSetValue(0, 9, 0);
		else
			GPIOSetValue(0, 9, 1);
		GPIOSetValue(2, 11, 0);
		GPIOSetValue(2, 11, 1);
	}
	GPIOSetValue(1, 11, 1);
}

void
Init7Segment(void)
{
	// Turn off 7-segment display
	GPIOSetDir(0, 9, 1); // MOSI
	GPIOSetDir(2, 11, 1); // CLK
	GPIOSetDir(1, 11, 1); // CS
	GPIOSetValue(1, 11, 0);
	SetSegment(-1);
}
*/

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
	Led::setOutput();
	
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
