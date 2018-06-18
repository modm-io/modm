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

	MODM_LOG_INFO << "Nucleo L432KC PWM example" << modm::endl;

	LedD13::setOutput(modm::Gpio::Low);

	uint32_t counter = 0;

	Timer1::connect<GpioOutputA8::Ch1, GpioOutputA9::Ch2,
					GpioOutputA10::Ch3, GpioOutputA11::Ch4>();

	Timer1::enable();
	Timer1::setMode(Timer1::Mode::UpCounter);

	// 80 MHz / 800 / 2^16 ~ 1.5 Hz
	Timer1::setPrescaler(800);
	Timer1::setOverflow(65535);


	Timer1::configureOutputChannel(1, Timer1::OutputCompareMode::Pwm, 32767);
	Timer1::configureOutputChannel(2, Timer1::OutputCompareMode::Pwm, 32767);
	Timer1::configureOutputChannel(3, Timer1::OutputCompareMode::Pwm, 32767);
	Timer1::configureOutputChannel(4, Timer1::OutputCompareMode::Pwm, 32767);

	Timer1::applyAndReset();

	Timer1::start();
	Timer1::enableOutput();

	while (true)
	{
		LedD13::toggle();
		modm::delayMilliseconds(500);
		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		MODM_LOG_INFO << "Timer1 counter: " << Timer1::getValue() << modm::endl;
	}

	return 0;
}
