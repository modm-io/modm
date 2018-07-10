/*
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using namespace Board;

#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	MODM_LOG_INFO << "Nucleo L432KC Advanced PWM example" << modm::endl << modm::endl;
	MODM_LOG_INFO << "Connect an oscilloscope to \"D9\"/GpioA8 (Timer Ch1)," << modm::endl;
	MODM_LOG_INFO << "\"A6\"/GpioA7 (Timer Ch1n complementary) and \"D13\"/GpioB3 (trigger out)." << modm::endl;
	MODM_LOG_INFO << "Setup the oscilloscope to trigger on \"D13\"," << modm::endl;
	MODM_LOG_INFO << "this pin generates a rising edge every second when to PWM state begins." << modm::endl;
	MODM_LOG_INFO << "The following states are configured sequentially (each for 250 ms):" << modm::endl;
	MODM_LOG_INFO << " * PWM" << modm::endl;
	MODM_LOG_INFO << " * High-Z" << modm::endl;
	MODM_LOG_INFO << " * High" << modm::endl;
	MODM_LOG_INFO << " * Low " << modm::endl;

	using TriggerOut = LedD13;
	TriggerOut::setOutput(modm::Gpio::Low);

	Timer1::connect<GpioOutputA8::Ch1, GpioOutputA7::Ch1n>();

	Timer1::enable();
	Timer1::setMode(Timer1::Mode::UpCounter);

	// 80 MHz / 80 / 2^16 = ~15 Hz
	Timer1::setPrescaler(80);
	Timer1::setOverflow(65535);
	Timer1::start();
	Timer1::enableOutput();

	constexpr uint32_t compareValue = 32767;

	uint32_t state = 0;

	while (true)
	{
		switch(state) {
			case 0:
				// Trigger only on state 0
				TriggerOut::set();

				MODM_LOG_INFO << "Mode: PWM" << modm::endl;
				Timer1::configureOutputChannel(1,
						Timer1::OutputCompareMode::Pwm,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveHigh,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveHigh,
						Timer1::OutputComparePreload::Disable
						);
				break;
			case 1:
				MODM_LOG_INFO << "Mode: HiZ" << modm::endl;
				Timer1::configureOutputChannel(1,
						Timer1::OutputCompareMode::ForceActive,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveLow,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveHigh,
						Timer1::OutputComparePreload::Disable
						);
				break;
			case 2:
				MODM_LOG_INFO << "Mode: High" << modm::endl;
				Timer1::configureOutputChannel(1,
						Timer1::OutputCompareMode::ForceActive,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveHigh,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveHigh,
						Timer1::OutputComparePreload::Disable
						);
				break;
			case 3:
				MODM_LOG_INFO << "Mode: Low" << modm::endl;
				Timer1::configureOutputChannel(1,
						Timer1::OutputCompareMode::ForceActive,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveLow,
						Timer1::PinState::Enable,
						Timer1::OutputComparePolarity::ActiveLow,
						Timer1::OutputComparePreload::Disable
						);
				break;
		}
		Timer1::setCompareValue(1, compareValue);
		Timer1::applyAndReset();

		// Go to next state after 250 ms
		if(++state > 3) {
			state = 0;
		}
		modm::delayMilliseconds(250);

		TriggerOut::reset();
	}

	return 0;
}
