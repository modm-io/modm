#ifndef ANIMATIONS_HPP
#define ANIMATIONS_HPP

#include "leds.hpp"
#include <xpcc/ui/animation.hpp>

// apply some animations to the leds
xpcc::ui::Pulse<uint8_t> pulse(red.animation);
xpcc::ui::Indicator<uint8_t> indicator(blue.animation);
xpcc::ui::Strobe<uint8_t> strobe(green.animation);
// ----------------------------------------------------------------------------

// Now for some custom animators

// custom keyframes (time, value) for the orange led
xpcc::ui::KeyFrame<uint8_t> frames[] =
{
		xpcc::ui::KeyFrame<uint8_t>(1000, 50),
		xpcc::ui::KeyFrame<uint8_t>(900, 0),
		xpcc::ui::KeyFrame<uint8_t>(800, 100),
		xpcc::ui::KeyFrame<uint8_t>(700, 0),
		xpcc::ui::KeyFrame<uint8_t>(600, 150),
		xpcc::ui::KeyFrame<uint8_t>(500, 0),
		xpcc::ui::KeyFrame<uint8_t>(400, 200),
		xpcc::ui::KeyFrame<uint8_t>(300, 0),
		xpcc::ui::KeyFrame<uint8_t>(200, 250),
		xpcc::ui::KeyFrame<uint8_t>(100, 0),
};
// create a new keyframe animator for the orange led
xpcc::ui::KeyFrameAnimation<uint8_t> animator(frames, 10, orange.animation);

// animate the period of the red pulse (Aniception?)
uint16_t period = 500;
xpcc::ui::Animation<uint16_t> periodAnimator(period);
// wrap it in a pulse
xpcc::ui::Pulse<uint16_t> pulsePeriod(periodAnimator);

#endif // ANIMATIONS_HPP
