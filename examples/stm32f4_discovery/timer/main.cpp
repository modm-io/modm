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
#include <modm/ui/led.hpp>
#include <modm/ui/animation.hpp>

// create the leds with these lambda callbacks
modm::ui::Led orange([](uint8_t brightness)
{
	Timer4::setCompareValue(2, modm::ui::table22_16_256[brightness]);
});

modm::ui::Led red([](uint8_t brightness)
{
	Timer4::setCompareValue(3, modm::ui::table22_16_256[brightness]);
});

modm::ui::Led green([](uint8_t brightness)
{
	Timer4::setCompareValue(1, modm::ui::table22_16_256[brightness]);
});

modm::ui::Led blue([](uint8_t brightness)
{
	Timer4::setCompareValue(4, modm::ui::table22_16_256[brightness]);
});
// ----------------------------------------------------------------------------

// apply some animations to the leds
modm::ui::Pulse<uint8_t> pulse(red);
modm::ui::Indicator<uint8_t> indicator(blue);
modm::ui::Strobe<uint8_t> strobe(green);

// Now for some custom animators
using KeyFrame = modm::ui::KeyFrame<uint8_t>;
using KeyFrameAnimation = modm::ui::KeyFrameAnimation<uint8_t>;

// custom keyframes (time, value) for the orange led
const KeyFrame frames[] =
{
	KeyFrame(1000, 50),
	KeyFrame(900, 0),
	KeyFrame(800, 100),
	KeyFrame(700, 0),
	KeyFrame(600, 150),
	KeyFrame(500, 0),
	KeyFrame(400, 200),
	KeyFrame(300, 0),
	KeyFrame(200, 250),
	KeyFrame(100, 0)
};
// create a new keyframe animator for the orange led
KeyFrameAnimation keyFrames(frames, orange);

// animate the period of the red pulse (Aniception?)
static uint16_t period = 500;
modm::ui::Animation<uint16_t> periodAnimator(period, [](uint16_t period)
{
	pulse.setPeriod(period);
});
// wrap it in a pulse
modm::ui::Pulse<uint16_t> pulsePeriod(periodAnimator);

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
	keyFrames.setMode(modm::ui::KeyFrameAnimationMode::Autoreverse);
	// set the indicator period change to 15s
	pulsePeriod.setPeriod(10000);
	// pulse between 0.5s and 5s.
	pulsePeriod.setRange(500, 5000);
	indicator.setRange(0, 100);

	// start all animations indefinitely
	pulse.start();
	indicator.start();
	strobe.start();
	keyFrames.start();
	pulsePeriod.start();

	while (true)
	{
		// update all standard animations
		pulse.update();
		indicator.update();
		strobe.update();

		// update the custom animations
		keyFrames.update();
		pulsePeriod.update();
	}

	return 0;
}
