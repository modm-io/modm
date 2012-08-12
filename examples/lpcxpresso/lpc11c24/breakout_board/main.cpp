
#include <xpcc/architecture.hpp>

#include "hardware.hpp"
#include "config.hpp"

#define LED_TOGGLE_TICKS 150		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

int
main(void)
{
	Hardware::initialize();
	
	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	while (1)
	{
		// Each time we wake up...
		// set the Leds
		if (button::Ok::read())
		{
			led::DuoRed::set();
			led::DuoGreen::set();
		}
		else {
			led::DuoRed::set(button::Up::read());
			led::DuoGreen::set(button::Down::read());
		}

		// Check TimeTick to see whether to set or clear the LED I/O pin
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX)) < ((LED_TOGGLE_TICKS / COUNT_MAX) / 2)) {
			led::Onboard::reset();
			led::Xpresso::set();
		}
		else {
			led::Onboard::set();
			led::Xpresso::reset();

			// Write four digits, from right to left.
			// ssd::Ssd::writeHex(timer32_0_counter);
			if (adc::Adc::isConversionFinished()) {
				ssd::Ssd::write(adc::Adc::getValue());
				adc::Adc::startConverstion(adc::Adc::ChannelMask::PIO1_4);
			}
		}
		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}
