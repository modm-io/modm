#ifndef ANIMATIONS_HPP
#define ANIMATIONS_HPP

#include "leds.hpp"
#include <xpcc/ui/animation.hpp>

// apply some animations to the leds
xpcc::ui::Pulse<uint8_t> pulse(red);
xpcc::ui::Indicator<uint8_t> indicator(blue);
xpcc::ui::Strobe<uint8_t> strobe(green);
// ----------------------------------------------------------------------------

// Now for some custom animators
using KeyFrame = xpcc::ui::KeyFrame<uint8_t>;
using KeyFrameAnimation = xpcc::ui::KeyFrameAnimation<uint8_t>;

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
xpcc::ui::Animation<uint16_t> periodAnimator(period);
// wrap it in a pulse
xpcc::ui::Pulse<uint16_t> pulsePeriod(periodAnimator);

#endif // ANIMATIONS_HPP
