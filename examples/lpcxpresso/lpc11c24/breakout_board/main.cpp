
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <xpcc/math.hpp>

#include "hardware.hpp"
#include "config.hpp"

#include "logger.hpp" // moves to xpcc as soon as it is finished

static uint16_t servoSetPosition = 511;

static xpcc::Logger::OutputStream<logger::Sample, 1024> loggerStream;

enum class AdcState
{
	CURRENT,
	POSITION
};

static AdcState adcState = AdcState::CURRENT;

MAIN_FUNCTION
{
	Hardware::initialize();
	
	xpcc::Timeout<> timeout(10);

	xpcc::Logger::initialize();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
//	init_timer32(0, TIME_INTERVAL);

	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
//	enable_timer32(0);

	while (1)
	{
		// Process logger, includes sending data
//		xpcc::Logger::update();
		loggerStream.update();

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
//			ssd::Ssd::write(setVal);

			if (button::Ok::read()) {
				// block keys for 0.5 sec, otherwise triggering more than once
				timeout.restart(500);

				// set servo
				servoSetPosition = setVal;

				// trigger logger
				// 20 samples before current trigger point are kept.
				loggerStream.trigger(20);
			}
		}

	}
}


extern "C" void
ADC_IRQHandler(void) {
	led::Onboard::set();

	switch (adcState)
	{
	case AdcState::CURRENT:
	{
//		led::Onboard::toggle();
//		led::Onboard::toggle();

		// Read current and clear individual interrupt mask
		uint16_t current = adc::Adc::getValue(adc::Adc::Channel::CHANNEL_1);

		loggerStream.channel.current = current;

		/**
		 * TODO: Current limiter
		 */

		// display motor current
		static uint16_t dd = 0;
		if (++dd == 2400) {
			dd = 0;
			ssd::Ssd::write(current);
		}



		// Now start a conversion of the position.
		adc::Adc::startConversion(adc::Adc::ChannelMask::CHANNEL_3);

		adcState = AdcState::POSITION;
	}
	break;

	case AdcState::POSITION:
	{
//		led::Onboard::toggle();
//		led::Onboard::toggle();
//		led::Onboard::toggle();
//		led::Onboard::toggle();

		// Read position and clear individual interrupt mask
		// Cast to signed for less problems when subtracting
		int16_t position = adc::Adc::getValue(adc::Adc::Channel::CHANNEL_3);
		loggerStream.channel.position = position;


		// display position
//		static uint16_t dd = 0;
//		if (++dd == 24000) {
//			dd = 0;
//			ssd::Ssd::write(position);
//		}

//		static uint8_t delay = 0;

//		if ((++delay % 24) == 0) {
			if (abs(position - servoSetPosition) > 50) {
				// something to do
				if (position > servoSetPosition) {
					servo::EnB::reset();
					servo::pwmTimer::setMatchValue(0, 2000); // off

					// PwmA = Ch0
					servo::pwmTimer::setMatchValue(1, 500);
					servo::EnA::set();
				}
				else if (position < servoSetPosition) {
					servo::EnA::reset();
					servo::pwmTimer::setMatchValue(1, 2000);

					// PwmA = Ch0
					servo::pwmTimer::setMatchValue(0, 500);
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
//		} // delay

		// next conversion is triggered by timer and will be the current measurement.
		adcState = AdcState::CURRENT;
		break;
	}
	} // switch

	led::Onboard::reset();
}

extern "C" void
TIMER16_0_IRQHandler() __attribute__ ((section (".fastcode")));

extern "C" void
TIMER16_0_IRQHandler()
{
	// write all old values to stream
	loggerStream.write();

	// Start the ADC to measure current
	adc::Adc::startConversion(adc::Adc::ChannelMask::CHANNEL_1);

	led::Xpresso::set();

	// reset timer IRQ bit
	LPC_TMR16B0->IR = 0xff;

	led::Xpresso::reset();

} // IRQ



