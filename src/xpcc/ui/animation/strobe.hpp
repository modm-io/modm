// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_STROBE_HPP
#define XPCC_UI_STROBE_HPP

#include <stdint.h>
#include "key_frame.hpp"
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{

namespace ui
{

/**
 * Strobe animation.
 *
 * Animates a value like this:
 *   _   _
 * _/ \_/ \__________
 *
 * @author	Niklas Hauser
 * @ingroup ui
 */
template< typename T = uint8_t >
class Strobe
{
public:
	Strobe(Animation<T> &animator)
	:	animator(animator), frames{
			xpcc::ui::KeyFrame<T>(60, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(40, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(90, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(110, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(60, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(40, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(90, xpcc::ArithmeticTraits<T>::min),
			xpcc::ui::KeyFrame<T>(510, xpcc::ArithmeticTraits<T>::min),
		}
	{
		this->animator.setKeyFrames(frames, 8);
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
	// 2. stay at max (on1)
	// 3. fade down to min
	// 4. stay at min (pause)
	// 5. fade up to max
	// 6. stay at max (on2)
	// 7. fade down to min
	// 8. stay at min
	// (cycle)
	xpcc::ui::KeyFrame<T> frames[8];
};

}

}

#endif	// XPCC_UI_STROBE_HPP
