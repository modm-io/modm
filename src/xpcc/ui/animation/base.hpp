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
	// for (u)int8_t types, the time is limited to ~32s anyway,
	// so we do not need uint32_t for the time, but we can use uint16_t
public:
	typedef typename xpcc::tmp::Select<
			xpcc::tmp::SameType<UnsignedType, uint8_t>::value,
			uint16_t,
			uint32_t >::Result TimeType;

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

	// the default implementation uses floating-point arithmetic
	template< typename Type, typename Unsigned >
	class Computations
	{
		float accumulatedValue;
		float deltaValue;

	public:
		void inline
		initialize(Type currentValue, Type endValue, uint32_t time)
		{
			accumulatedValue = static_cast<float>(currentValue);
			float delta = (static_cast<float>(endValue) - currentValue);
			deltaValue = delta / time;
			if (deltaValue == 0)
				deltaValue = delta > 0 ? xpcc::ArithmeticTraits<float>::epsilon : -xpcc::ArithmeticTraits<float>::epsilon;
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
		uint16_t accumulatedValue;
		int16_t deltaValue;

		void inline
		initialize(Type currentValue, Type endValue, uint16_t time)
		{
			accumulatedValue = static_cast<uint16_t>(currentValue) << 7;
			int16_t delta = (static_cast<int16_t>(endValue) - currentValue) << 7;
			deltaValue = delta / static_cast<int16_t>(time);
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
		uint32_t accumulatedValue;
		int32_t deltaValue;

		void inline
		initialize(Type currentValue, Type endValue, uint32_t time)
		{
			accumulatedValue = static_cast<uint32_t>(currentValue) << 15;
			int32_t delta = (static_cast<int32_t>(endValue) - currentValue) << 15;
			deltaValue = delta / static_cast<int32_t>(time);
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

	// On the AVR 64bit variables do not exits, therefore this must be excluded,
	// so that it may revert back to using floats.
	// It's not pretty, but neither is using uint32_t on an 8bit CPU to begin with.
#if !defined(XPCC__CPU_AVR)

	// uint32_t implementation using signed 32.16 fixed point arithmetic.
	// The maximum change can be +-2^32 since the value type is 32 bit wide.
	// The remaining 16 bits are used for fractional delta time:
	// 1 step can take at most 65356ms, which is equivalent to about 8900 years
	// over the whole range.
	// Note: we use 16 bits for the fractionals here, so that it is byte-aligned.
	// This should allow CPUs without a barrelshifter to copy the value instead of shifting it.
	template< typename Type >
	struct Computations <Type, uint32_t>
	{
		uint64_t accumulatedValue;
		int64_t deltaValue;

		void inline
		initialize(Type currentValue, Type endValue, uint32_t time)
		{
			accumulatedValue = static_cast<uint64_t>(currentValue) << 16;
			int64_t delta = (static_cast<int64_t>(endValue) - currentValue) << 16;
			deltaValue = delta / static_cast<int32_t>(time);
			if (deltaValue == 0)
				deltaValue = delta > 0 ? 1 : -1;
		}

		Type inline
		step()
		{
			accumulatedValue += deltaValue;
			return static_cast<Type>(accumulatedValue >> 16);
		}
	};
#endif

	// create an instance of the calculation helper
	Computations<T, UnsignedType> computations;
};

}	// namespace ui

}	// namespace xpcc

#include "base_impl.hpp"

#endif	// XPCC_UI_LINEAR_ANIMATION_HPP
