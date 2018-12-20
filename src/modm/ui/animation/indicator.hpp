/*
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_UI_INDICATOR_HPP
#define MODM_UI_INDICATOR_HPP

#include <stdint.h>
#include "key_frame.hpp"
#include <type_traits>

namespace modm
{

namespace ui
{

/**
 * This class provided smooth on and off phases, much like a
 * blinker/indicator on cars.
 *
 * @code
 *   |-------------- period -|
 *   |- duty cycle -|
 *    ______________          _____
 *  _/              \________/      ...
 *   ^ onFade       ^ offFade
 * @endcode
 *
 * @author	Niklas Hauser
 * @ingroup modm_ui_animation
 */
template< typename T = uint8_t >
class Indicator
{
	typedef typename Animation<T>::TimeType TimeType;
public:
	/// constructs an indicator with a period of 1s
	Indicator(Animation<T> &animator)
	:	animator(animator), frames{
			modm::ui::KeyFrame<T>(75, std::numeric_limits<T>::max()),
			modm::ui::KeyFrame<T>(375, std::numeric_limits<T>::max()),
			modm::ui::KeyFrame<T>(115, std::numeric_limits<T>::min()),
			modm::ui::KeyFrame<T>(435, std::numeric_limits<T>::min())
		}
	{
		this->animator.setKeyFrames(frames, 4);
		this->animator.setMode(modm::ui::KeyFrameAnimationMode::Repeat);
	}

	/// @param	period		in ms
	/// @param	dutyCycle	in percent <= 100
	inline void
	setPeriod(TimeType period, uint8_t dutyCycle=50)
	{
		if (dutyCycle > 100) dutyCycle = 100;
		frames[1].length = static_cast<uint32_t>(period) * dutyCycle / 100;
		// subtract the on and off fade
		uint32_t fadeLength = (frames[0].length + frames[2].length);
		if (frames[1].length < fadeLength) frames[1].length = 0;
		else frames[1].length -= fadeLength;

		frames[3].length = period - frames[1].length;
	}

	/// @param	onFade		the time is takes to turn on in ms
	/// @param	offFade		the time is takes to turn off in ms
	inline void
	setFadeTimes(TimeType onFade, TimeType offFade)
	{
		frames[1].length += (frames[0].length + frames[2].length);
		frames[0].length = onFade;
		frames[2].length = offFade;
		uint32_t fadeLength = (frames[0].length + frames[2].length);
		if (frames[1].length < fadeLength) frames[1].length = 0;
		else frames[1].length -= fadeLength;
	}

	void inline
	setRange(T minValue, T maxValue)
	{
		frames[0].value[0] = maxValue;
		frames[1].value[0] = maxValue;
		frames[2].value[0] = minValue;
		frames[3].value[0] = minValue;
	}

	/// start indicating for ever
	inline void
	start(uint8_t repeat=0)
	{
		animator.start(repeat);
	}

	inline void
	stop()
	{
		animator.stop();
	}

	inline bool
	isAnimating() const
	{
		return animator.isAnimating();
	}

	/// Must be called at least every ms
	void inline
	update()
	{
		animator.update();
	}

private:
	modm::ui::KeyFrameAnimation<T> animator;
	// 1. fade up to max
	// 2. stay at max
	// 3. fade down to min
	// 4. stay at min
	// (cycle)
	modm::ui::KeyFrame<T> frames[4];
};

}

}

#endif	// MODM_UI_INDICATOR_HPP
