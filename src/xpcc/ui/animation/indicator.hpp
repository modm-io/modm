// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_INDICATOR_HPP
#define XPCC_UI_INDICATOR_HPP

#include <stdint.h>
#include "key_frame.hpp"
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{

namespace ui
{

/**
 * Indicator.
 *
 * This class provided smooth on and off phases, much like a
 * blinker/indicator on cars.
 *
 *   ______
 * _/      \________
 *
 * @author	Niklas Hauser
 * @ingroup ui
 */
template< typename T = uint8_t >
class Indicator
{
public:
	Indicator(Animation<T> &animator)
	:	animator(animator), frames{
			xpcc::ui::KeyFrame<T>(75, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(375, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(115, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(435, xpcc::ArithmeticTraits<T>::min)
		}
	{
		this->animator.setKeyFrames(frames, 4);
		this->animator.setMode(xpcc::ui::KeyFrameAnimationMode::Cycle);
	}

	/// set new period
	inline void
	setPeriod(typename Animation<T>::TimeType period, uint8_t dutyCycle=45)
	{
		frames[1].time = period * dutyCycle / 100;
		frames[3].time = period - frames[1].time - frames[2].time;
		frames[1].time -= frames[0].time;
	}

	inline void
	setFadeTimes(typename Animation<T>::TimeType onFadeTime,
				 typename Animation<T>::TimeType offFadeTime)
	{
		frames[1].time += frames[0].time;
		frames[3].time += frames[2].time;
		frames[0].time = onFadeTime;
		frames[2].time = offFadeTime;
		frames[1].time -= frames[0].time;
		frames[3].time -= frames[2].time;
	}

	void inline
	setRange(T minValue, T maxValue)
	{
		frames[0].value = maxValue;
		frames[1].value = maxValue;
		frames[3].value = minValue;
		frames[4].value = minValue;
	}

	/// start indicating for ever
	inline void
	start(uint8_t repeat=-1)
	{
		animator.start(repeat);
	}

	/// Stops indicating after finishing the current cycle
	inline void
	stop()
	{
		animator.stop();
	}

	inline void
	cancel()
	{
		animator.cancel();
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
	xpcc::ui::KeyFrameAnimation<T> animator;
	// 1. fade up to max
	// 2. stay at max
	// 3. fade down to min
	// 4. stay at min
	// (cycle)
	xpcc::ui::KeyFrame<T> frames[4];
};

}

}

#endif	// XPCC_UI_INDICATOR_HPP
