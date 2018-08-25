/*
 * Copyright (c) 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef ANIMATIONS_HPP
#define ANIMATIONS_HPP

#include "leds.hpp"
#include <modm/ui/animation.hpp>

// apply some animations to the leds
modm::ui::Pulse<uint8_t> pulse(red);
modm::ui::Indicator<uint8_t> indicator(blue);
modm::ui::Strobe<uint8_t> strobe(green);
// ----------------------------------------------------------------------------

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
KeyFrameAnimation animator(frames, orange);

// animate the period of the red pulse (Aniception?)
static uint16_t period = 500;
modm::ui::Animation<uint16_t> periodAnimator(period);
// wrap it in a pulse
modm::ui::Pulse<uint16_t> pulsePeriod(periodAnimator);

#endif // ANIMATIONS_HPP
