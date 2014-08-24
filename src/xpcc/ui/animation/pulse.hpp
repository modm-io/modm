// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_PULSE_HPP
#define XPCC_UI_PULSE_HPP

#include <stdint.h>
#include "key_frame.hpp"
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{
namespace ui
{
/**
 * Pulser.
 *
 * This class animates a value up and down during the period.
 *
 * @see		Led
 * @author	Niklas Hauser
 * @ingroup led
 */
template< typename T = uint8_t >
class Pulse
{
public:
	Pulse(Animation<T> &animator)
	:	animator(animator), frames{
			xpcc::ui::KeyFrame<T>(500, xpcc::ArithmeticTraits<T>::max),
			xpcc::ui::KeyFrame<T>(500, xpcc::ArithmeticTraits<T>::min)
		}
	{
		this->animator.setKeyFrames(frames, 2);
		this->animator.setMode(xpcc::ui::KeyFrameAnimationMode::Cycle);
	}

	/// set new period
	inline void
	setPeriod(typename Animation<T>::TimeType period, uint8_t dutyCycle=50)
	{
		frames[0].time = period * dutyCycle / 100;
		frames[1].time = period - frames[0].time;
	}

	void inline
	setRange(T minValue, T maxValue)
	{
		frames[0].value = maxValue;
		frames[1].value = minValue;
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
	// 2. fade down to min
	// (cycle)
	xpcc::ui::KeyFrame<T> frames[2];
};

}

}

#endif	// XPCC_UI_PULSE_HPP
