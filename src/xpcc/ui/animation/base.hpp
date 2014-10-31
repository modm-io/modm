// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_LINEAR_ANIMATION_HPP
#define XPCC_UI_LINEAR_ANIMATION_HPP

#include <stdint.h>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>
#include "interpolation.hpp"

namespace xpcc
{

namespace ui
{

/**
 * This class allows the linear animation of one value over time.
 *
 * All integer types use binary scaling through a fixed-point
 * arithmetic, however, all other types default to floating-point
 * arithmetic with casting between types.
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
 * @ingroup ui
 */
template< typename T = uint8_t >
class Animation
{
private:
	typedef typename xpcc::ArithmeticTraits<T>::UnsignedType UnsignedType;
public:
	typedef typename LinearInterpolation<T>::StepType TimeType;

	Animation(T &value);

	void
	setValue(T value);

	/// @return the current value
	T
	getValue() const;

	/// @return `true` if animation is currently running,
	///			`false` if otherwise
	bool
	isAnimating() const;

	/// stops the current running animation.
	void
	stop();

	/// Animate from the current value to a new value in the specified ms.
	/// @return `true` if animation started,
	///			`false` if otherwise
	bool
	animateTo(TimeType time, T value);

	/// Can be called at an interval of 1ms or less.
	/// If you do not need 1ms response time , you can calls this at larger intervals.
	/// @return	`true` if the value has been changed,
	///			`false` otherwise
	bool
	update();

private:

	T &currentValue;
	T endValue;
	TimeType animationTime;
	xpcc::Timestamp previous;
	// create an instance of the calculation helper
	LinearInterpolation<T> interpolation;
};

}	// namespace ui

}	// namespace xpcc

#include "base_impl.hpp"

#endif	// XPCC_UI_LINEAR_ANIMATION_HPP
