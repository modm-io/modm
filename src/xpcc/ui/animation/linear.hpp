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
#include <xpcc/processing/periodic_timer.hpp>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{

namespace ui
{

/**
 * This class allows the linear animation of one value over time.
 *
 * The types int8_t/uint8_t and int16_t/uint16_t use a fast fixed-point
 * arithmetic, however, all other types default to floating-point
 * arithmetic with casting between types.
 *
 * @warning	Be aware that the algortihm for 8bit types limits the animation length
 * 			to `128ms * value_difference`, which is about 32s over the full 8bit range.
 * 			If you specify a longer time in this case, the animation simply finishes before.
 * 			If this is a problem, consider using a 16bit type, which does not have
 * 			this limitation.
 *
 * @author	Niklas Hauser
 * @ingroup ui
 */
template< typename T = uint8_t >
class LinearAnimation
{
public:
	LinearAnimation(T &value);

	void
	setValue(T value);

	/// @return the current value
	T
	getValue() const;

	/// @return `true` if animation is currently running,
	///			`false` if otherwise
	bool
	isAnimating() const;

	/// Animate from the current value to a new value in the specified ms.
	/// Animation times are limited to ~32s.
	/// If you require a slower animation, you need to do this externally.
	/// @return `true` if animation started,
	///			`false` if otherwise
	bool
	animateTo(uint16_t time, T value);

	/// Can be called at a interval of 1ms or less.
	/// If you do not need 1ms response time , you can calls this at larger intervals.
	/// @return	`true` if the value has been changed,
	///			`false` otherwise
	bool
	update();

private:
	T &currentValue;
	T endValue;
	uint16_t animationTime;
	xpcc::Timestamp previous;

	typedef typename xpcc::ArithmeticTraits<T>::WideType WideType;
	typedef typename xpcc::ArithmeticTraits<T>::UnsignedType UnsignedType;
	typedef typename xpcc::ArithmeticTraits<T>::SignedType SignedType;
	typedef typename xpcc::ArithmeticTraits<SignedType>::WideType SignedWideType;

	// the default implementation uses floating-point arithmetic
	template< typename Type, typename Unsigned >
	class Computations
	{
		float accumulatedValue;
		float deltaValue;

	public:
		void inline
		initialize(Type currentValue, Type endValue, uint16_t time)
		{
			accumulatedValue = currentValue;
			float delta = (static_cast<float>(endValue) - currentValue);
			deltaValue = delta / time;
			if (deltaValue == 0)
				deltaValue = delta > 0 ? xpcc::ArithmeticTraits<float>::epsilon() : -xpcc::ArithmeticTraits<float>::epsilon();
		}

		Type inline
		step()
		{
			accumulatedValue += deltaValue;
			return static_cast<Type>(accumulatedValue);
		}
	};

	// uint8_t implementation using signed 8.7 fixed-point arithmetic.
	// The maximum change can be +-255 since the value type is 8 bit wide.
	// The remaining 7 bits are used for fractional delta time:
	// 1 step can take at most 128ms, which is equivalent to about 32s
	// over the whole range.
	template< typename Type >
	struct Computations <Type, uint8_t>
	{
		WideType accumulatedValue;
		SignedWideType deltaValue;

		void inline
		initialize(Type currentValue, Type endValue, uint16_t time)
		{
			accumulatedValue = static_cast<WideType>(currentValue) << 7;
			SignedWideType delta = (static_cast<SignedWideType>(endValue) - currentValue) << 7;
			deltaValue = delta / time;
			if (deltaValue == 0)
				deltaValue = delta > 0 ? 1 : -1;
		}

		Type inline
		step()
		{
			accumulatedValue += deltaValue;
			return static_cast<Type>(accumulatedValue >> 7);
		}
	};

	// uint16_t implementation using signed 16.15 fixed point arithmetic.
	// The maximum change can be +-65535 since the value type is 16 bit wide.
	// The remaining 15 bits are used for fractional delta time:
	// 1 step can take at most 32768ms, which is equivalent to about 24 days
	// over the whole range.
	template< typename Type >
	struct Computations <Type, uint16_t>
	{
		WideType accumulatedValue;
		SignedWideType deltaValue;

		void inline
		initialize(Type currentValue, Type endValue, uint16_t time)
		{
			accumulatedValue = static_cast<WideType>(currentValue) << 15;
			SignedWideType delta = (static_cast<SignedWideType>(endValue) - currentValue) << 15;
			deltaValue = delta / time;
			if (deltaValue == 0)
				deltaValue = delta > 0 ? 1 : -1;
		}

		Type inline
		step()
		{
			accumulatedValue += deltaValue;
			return static_cast<Type>(accumulatedValue >> 15);
		}
	};

	// create an instance of the calculation helper
	Computations<T, UnsignedType> computations;
};

}	// namespace ui

}	// namespace xpcc

#include "linear_impl.hpp"

#endif	// XPCC_UI_LINEAR_ANIMATION_HPP
