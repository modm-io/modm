/*
 * Copyright (c) 2023, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	using Timer = Timer1;
	// using Pin  	= D7;
	using PinN = D11;

	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter);
	// Timer clock: APB2 timer clock (170MHz)
	Timer::setPrescaler(10);
	// Prescaler: 1 -> Timer counter frequency: 170MHz
	static constexpr auto maxPwm = 4250;
	Timer::setOverflow(4250);
	// Pwm frequency: 170MHz / 4250 / 10 = 4kHz

	Timer::configureOutputChannel<PinN::Ch1n>(Timer::OutputCompareMode::Pwm, 0);

	// Alternative way to configure this:
	// Timer::configureOutputChannel(1, Timer::OutputCompareMode::Pwm, 0,Timer::PinState::Enable,
	// Timer::ComplementaryChannel::ChN);

	// Alternative way to configure this:
	// Timer::configureOutputChannel(1,
	// 	Timer::OutputCompareMode::Pwm,
	// 	Timer::PinState::Disable, // disable Ch1 output
	// 	Timer::OutputComparePolarity::ActiveHigh, // this doesn't matter
	// 	Timer::PinState::Enable, // enable Ch1n output
	// 	Timer::OutputComparePolarity::ActiveHigh, // OutputComparePolarity for Ch1n
	// 	Timer::OutputComparePreload::Disable
	// 	);

	Timer::applyAndReset();
	Timer::setCompareValue(1, 0);
	Timer::applyAndReset();
	Timer::pause();
	Timer::enableOutput();
	Timer::connect<PinN::Ch1n>();  // it works to just connect the Ch1n
	Timer::start();
	Timer::setCompareValue(1, maxPwm / 2);

	while (true)
	{
		LedD13::toggle();
		modm::delay(100ms);
	}

	return 0;
}
