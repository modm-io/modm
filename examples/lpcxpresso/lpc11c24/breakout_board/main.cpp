
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

			xpcc::can::Message message;
			message.identifier = 0x123456;
			message.length = 8;
			message.setRemoteTransmitRequest(false);
			message.data[0] = 'A';
			message.data[1] = 'B';
			message.data[2] = 'C';
			message.data[3] = 'D';
			message.data[4] = 'E';
			message.data[5] = 'F';
			message.data[6] = 'G';
			message.data[7] = 'H';
			if (can::Can::isReadyToSend()) {
				can::Can::sendMessage(message);
			}
		}
		else {
			led::DuoRed::set(button::Up::read());
			led::DuoGreen::set(button::Down::read());
		}

		// Check TimeTick to see whether to set or clear the LED I/O pin
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX)) < ((LED_TOGGLE_TICKS / COUNT_MAX) / 2)) {
			led::Onboard::reset();
			led::Xpresso::set();

			if (can::Can::isMessageAvailable()) {
				xpcc::can::Message message;
				can::Can::getMessage(message);
				ssd::Ssd::writeHex(message.data[0]);
			}
		}
		else {
			led::Onboard::set();
			led::Xpresso::reset();

			uint16_t val;
			if (adc::Adc::read(val, adc::Adc::Channel::PIO1_4)) {
				ssd::Ssd::write(val);
			}
		}
		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}
