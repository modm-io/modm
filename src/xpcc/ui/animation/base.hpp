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

#ifndef XPCC_UI_LINEAR_ANIMATION_HPP
#define XPCC_UI_LINEAR_ANIMATION_HPP

#include <stdint.h>
#include <modm/architecture/driver/clock.hpp>
#include <modm/utils/arithmetic_traits.hpp>
#include "interpolation.hpp"

namespace xpcc
{

namespace ui
{

/**
 * This class allows the linear animation of one value over time.
 *
 * Be aware that the algortihm for 8bit types is optimized for low computational costs,
 * developed for fast LED fading (@see xpcc::ui::Led).
 * Therefore the animation length is limited to `128ms * value_difference`, which is
 * about 32s over the full 8bit range.
 * If you specify a longer time in this case, the animation simply finishes before.
 * If this is a problem, consider using a 16bit type, which does not have
 * this limitation.
 *
 * @author	Niklas Hauser
 * @ingroup animation
 */
template< typename T = uint8_t >
class Animation
{
public:
	typedef typename FastRamp<T>::StepType TimeType;
	using Handler = void(*)(T);

public:
	/// @param	value	a reference to the value to be animated
	Animation(T &value);

	/// @param	value		a reference to the value to be animated
	/// @param	handler		this function will be called whenever the value has been changed
	Animation(T &value, Handler handler);

	/// @param	handler		this function will be called whenever the value has been changed
	void
	attachCallback(Handler handler);

	/// stop any running animation and set a value.
	void
	setValue(T value);

	/// @return the current value
	T
	getValue() const;

	/// @return `true` if animation is currently running,
	///			`false` if otherwise
	bool
	isAnimating() const;

	/// stops any running animation.
	void
	stop();

	/// Animate from the current value to a new value in the specified ms.
	/// @return `true` if animation started,
	///			`false` if otherwise
	bool
	animateTo(T value, TimeType time);

	/// Can be called at an interval of 1ms or less.
	/// If you do not need 1ms response time, you may call this at intervals < 250ms.
	/// @return	`true` if the value has been changed,
	///			`false` otherwise
	bool
	update();

private:
	Handler handler;
	T &currentValue;
	T endValue;
	TimeType animationTime;
	xpcc::ShortTimestamp previous;
	FastRamp<T> interpolation;
};

}	// namespace ui

}	// namespace xpcc

#include "base_impl.hpp"

#endif	// XPCC_UI_LINEAR_ANIMATION_HPP
