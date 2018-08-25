/*
 * Copyright (c) 2013, Fabian Greif
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2014-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include "leds.hpp"
#include "animations.hpp"

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// connect the Timer Channels to the LEDs
	Timer4::connect<
		Board::LedGreen::Ch1,
		Board::LedOrange::Ch2,
		Board::LedRed::Ch3,
		Board::LedBlue::Ch4>();

	// set up the timer for 16bit PWM
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);

	// 42 MHz / 1 / 2^16 ~ 640 Hz refresh rate
	Timer4::setPrescaler(1);
	Timer4::setOverflow(65535);
	// configure the output channels
	Timer4::configureOutputChannel(1, Timer4::OutputCompareMode::Pwm, 0);
	Timer4::configureOutputChannel(2, Timer4::OutputCompareMode::Pwm, 0);
	Timer4::configureOutputChannel(3, Timer4::OutputCompareMode::Pwm, 0);
	Timer4::configureOutputChannel(4, Timer4::OutputCompareMode::Pwm, 0);
	Timer4::applyAndReset();
	// start the timer
	Timer4::start();

	// set the animation mode for autoreverse the keyframes
	animator.setMode(modm::ui::KeyFrameAnimationMode::Autoreverse);
	// set the indicator period change to 15s
	pulsePeriod.setPeriod(10000);
	// pulse between 0.5s and 5s.
	pulsePeriod.setRange(500, 5000);
	indicator.setRange(0, 100);

	// start all animations indefinitely
	pulse.start();
	indicator.start();
	strobe.start();
	animator.start();
	pulsePeriod.start();

	while (1)
	{
		// update all standard animations
		pulse.update();
		indicator.update();
		strobe.update();

		// udpate the custoom animations
		animator.update();
		pulsePeriod.update();

		// update all leds
		blue.update();
		red.update();
		orange.update();
		green.update();

		// udpate the custom animator
		if (periodAnimator.update())
		{
			// a new value is available
			pulse.setPeriod(period);
		}
	}

	return 0;
}
