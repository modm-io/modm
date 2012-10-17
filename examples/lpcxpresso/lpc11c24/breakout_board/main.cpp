
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/timeout.hpp>
#include <xpcc/math.hpp>

#include "hardware.hpp"
#include "config.hpp"

#include "logger.hpp" // moves to xpcc as soon as it is finished

static uint16_t servoSetPosition = 511;

static xpcc::Logger::OutputStream<logger::Sample, 512> loggerStream;

enum class AdcState
{
	CURRENT,
	POSITION
};

static AdcState adcState = AdcState::CURRENT;

static const int32_t fir_coeffs[] = {
		  -2037,   -1126,   1258,   2852,
		   1645,   -1945,  -4753,  -3056,
		   4278,   14260,  21391,  21391,
		  14260,    4278,  -3055,  -4753,
		  -1944,    1645,   2852,   1258,
		  -1125,   -2037};

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
		// Read current and clear individual interrupt mask
		uint16_t current = adc::Adc::getValue(adc::Adc::Channel::CHANNEL_1);

		loggerStream.channel.current = current;

		/**
		 * Current filtering, FIR
		 */

		static int32_t fir_state[22] = {0};

		fir_state[0] = current << 16;

		int32_t acc = 0.0;
		for (uint8_t ii = 0; ii < 22; ++ii)
		{
			int32_t prod = (fir_coeffs[ii] >> 8) * (fir_state[ii] >> 8);
			acc += prod;
		}

		// shift
		for (uint8_t ii = 21; ii > 0; --ii)
		{
			fir_state[ii] = fir_state[ii - 1];
		}

		loggerStream.channel.current_filtered = acc >> 16;

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

		static uint8_t delay = 0;
		if (++delay == 100)
		{
			delay = 0;
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
		}

		// next conversion is triggered by timer and will be the current measurement.
		adcState = AdcState::CURRENT;
		break;
	}
	} // switch

	led::Onboard::reset();
}

// Interrupt handler in RAM for fastes performance
// extern "C" void
// TIMER16_0_IRQHandler() __attribute__ ((section (".fastcode")));

extern "C" void
TIMER16_0_IRQHandler()
{
	// write all old values to stream
	static uint8_t delay = 0;
	if (++delay == 2)
	{
		delay = 0;
		loggerStream.write();
	}

	// Start the ADC to measure current
	adc::Adc::startConversion(adc::Adc::ChannelMask::CHANNEL_1);

	led::Xpresso::set();

	// reset timer IRQ bit
	LPC_TMR16B0->IR = 0xff;

	led::Xpresso::reset();

} // IRQ



