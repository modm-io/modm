
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <xpcc/math.hpp>

#include "hardware.hpp"
#include "config.hpp"

#define LED_TOGGLE_TICKS 150		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

static uint16_t servoSetPosition = 511;

int
main(void)
{
	Hardware::initialize();
	
	xpcc::Timeout<> timeout(10);

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

		// use buttons Up and Down to set new setVal for servo. OK to confirm.
		static uint16_t setVal = 511;
		if (timeout.isExpired()) {
			timeout.restart(10);
			if (button::Up::read()) {
				if (setVal <= 1022) {
					++setVal;
				}
			}
			if (button::Down::read()) {
				if (setVal >= 1) {
					--setVal;
				}
			}
			ssd::Ssd::write(setVal);

			if (button::Ok::read()) {
				// set servo
				servoSetPosition = setVal;
			}
		}

		// Check TimeTick to see whether to set or clear the LED I/O pin
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX)) < ((LED_TOGGLE_TICKS / COUNT_MAX) / 2)) {
			led::Onboard::reset();
			led::Xpresso::set();

			if (can::Can::isMessageAvailable()) {
				xpcc::can::Message message;
				can::Can::getMessage(message);
//				ssd::Ssd::writeHex(message.data[0]);
			}
		}
		else {
			led::Onboard::set();
			led::Xpresso::reset();

		}
		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}


extern "C" void
ADC_IRQHandler(void) {

	static uint8_t delay = 0;

	uint16_t val;
	// always read to clear interrupt flag
	adc::Adc::read(val, adc::Adc::Channel::PIO1_2);
	if (++delay == 0) {
		int16_t position = val; // cast to signed for less problems when substracting

		if (abs(position - servoSetPosition) > 50) {
			// something to do
			if (position > servoSetPosition) {
				servo::EnB::reset();
				servo::pwmTimer::setMatchValue(0, 2000);

				// PwmA = Ch0
				servo::pwmTimer::setMatchValue(1, 1500);
				servo::EnA::set();
			}
			else if (position < servoSetPosition) {
				servo::EnA::reset();
				servo::pwmTimer::setMatchValue(1, 2000);

				// PwmA = Ch0
				servo::pwmTimer::setMatchValue(0, 1500);
				servo::EnB::set();
			}
		}
		else {
			// nothing to do
			servo::EnB::reset();
			servo::EnA::reset();
			servo::pwmTimer::setMatchValue(0, 2000);
			servo::pwmTimer::setMatchValue(1, 2000);
		}

		// Display Servo position
//		ssd::Ssd::write(val);
		uint8_t buf[10];
		uint8_t len;
		buf[0] = '#';
		buf[1] = val >> 8;
		buf[2] = val & 0xff;
		len = 3;

		static uint16_t d = 0;
		if (++d == 0) {
			usb::uart::write(buf[0]);
		}
	}
}
