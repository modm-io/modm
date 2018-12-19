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

#ifndef MODM_UI_PULSE_HPP
#define MODM_UI_PULSE_HPP

#include <stdint.h>
#include "key_frame.hpp"
#include <type_traits>

namespace modm
{

namespace ui
{

/**
 * This class animates a value up and down during the period.
 *
 * @code
 *  |--| duty cycle
 *     /\      /\
 *    /  \    /  \
 *   /    \  /    \
 *  /      \/      \ ...
 *  |------| period
 * @endcode
 *
 * @author	Niklas Hauser
 * @ingroup modm_ui_animation
 */
template< typename T = uint8_t >
class Pulse
{
	typedef typename Animation<T>::TimeType TimeType;
public:
	Pulse(Animation<T> &animator)
	:	animator(animator), frames{
			modm::ui::KeyFrame<T>(500, std::numeric_limits<T>::max()),
			modm::ui::KeyFrame<T>(500, std::numeric_limits<T>::min())
		}
	{
		this->animator.setKeyFrames(frames, 2);
	}

	/// @param	period		in ms
	/// @param	dutyCycle	in percent <= 100
	inline void
	setPeriod(TimeType period, uint8_t dutyCycle=50)
	{
		if (dutyCycle > 100) dutyCycle = 100;
		frames[0].length = static_cast<uint32_t>(period) * dutyCycle / 100;
		frames[1].length = period - frames[0].length;
	}

	void inline
	setRange(T minValue, T maxValue)
	{
		frames[0].value[0] = maxValue;
		frames[1].value[0] = minValue;
	}

	/// start indicating for ever
	inline void
	start(uint8_t repeat=0)
	{
		animator.start(repeat);
	}

	/// Stops indicating after finishing the current cycle
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
	// 2. fade down to min
	// (cycle)
	modm::ui::KeyFrame<T> frames[2];
};

}

}

#endif	// MODM_UI_PULSE_HPP
