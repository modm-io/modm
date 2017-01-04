/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_LINEAR_INTERPOLATION_HPP
#define XPCC_UI_LINEAR_INTERPOLATION_HPP

#include <stdint.h>
#include <modm/utils/arithmetic_traits.hpp>
#include <modm/utils/template_metaprogramming.hpp>

namespace xpcc
{

namespace ui
{

/**
 * This class allows the linear ramping of one value over a number of steps.
 *
 * All integer types use binary scaling through a fixed-point
 * arithmetic, however, all other types default to floating-point
 * arithmetic with casting between types.
 *
 * Be aware that the algortihm for 8bit types is optimized for low computational costs.
 * Therefore the steps are limited to `128 steps * value_difference`, which is
 * 32768 steps over the full 8bit range.
 * If you specify a more steps in this case, the ramp simply is 'steeper'.
 * If this is a problem, consider using a 16bit type, which does not have
 * this limitation.
 *
 * @warning	This class does not know when to stop stepping.
 * 			This means, that over- and underflows of the target value
 * 			must be prevented externally.
 *
 * @author	Niklas Hauser
 * @ingroup animation
 */
template< typename T = uint8_t >
class FastRamp
{
private:
	typedef typename xpcc::ArithmeticTraits<T>::UnsignedType UnsignedType;
public:
	/// for 8bit value types, the steps are limited to 2^15 anyway,
	/// so we do not need uint32_t for the steps, but we can use uint16_t
	typedef typename xpcc::tmp::Select<
			xpcc::tmp::SameType<UnsignedType, uint8_t>::value,
			uint16_t,
			uint32_t >::Result StepType;
private:
	/// the default implementation uses floating-point arithmetic
	template< typename Type, typename Unsigned >
	struct Computations
	{
		float accumulatedValue;
		float deltaValue;

		Computations()
		:	accumulatedValue(0), deltaValue(0)
		{
		}

		void inline
		initialize(Type begin, Type end, uint32_t steps)
		{
			float delta = (static_cast<float>(end) - begin);
			deltaValue = delta / steps;
			if (deltaValue == 0)
				deltaValue = delta > 0 ? xpcc::ArithmeticTraits<float>::epsilon : -xpcc::ArithmeticTraits<float>::epsilon;
			accumulatedValue = static_cast<float>(begin) + deltaValue / 2;
		}

		void inline
		step()
		{
			accumulatedValue += deltaValue;
		}

		Type inline
		get()
		{
			return static_cast<Type>(accumulatedValue);
		}
	};

	/**
	 * uint8_t implementation using signed 8.7 fixed-point arithmetic.
	 * The maximum change can be +-255 since the value type is 8 bit wide.
	 * The remaining 7 bits are used for fractional delta value:
	 * 1 value difference can take at most 128 steps, which is equivalent
	 * to 2^15 (32768) steps over the whole 8bit range.
	 */
	template< typename Type >
	struct Computations <Type, uint8_t>
	{
		// fast types get promoted to uint32 which makes this faster on ARM
		uint_fast16_t accumulatedValue;
		int_fast16_t deltaValue;

		Computations()
		:	accumulatedValue(0), deltaValue(0)
		{
		}

		void inline
		initialize(Type begin, Type end, uint_fast16_t steps)
		{
			int_fast16_t delta = (static_cast<int_fast16_t>(end) - begin) << 7;
			deltaValue = delta / static_cast<int_fast16_t>(steps);
			if (deltaValue == 0)
				deltaValue = delta > 0 ? 1 : -1;
			accumulatedValue = (static_cast<uint_fast16_t>(begin) << 7) + deltaValue / 2;
		}

		void inline
		step()
		{
			accumulatedValue += deltaValue;
		}

		Type inline
		get()
		{
			return static_cast<Type>(accumulatedValue >> 7);
		}
	};

	/**
	 * uint16_t implementation using signed 16.15 fixed point arithmetic.
	 * The maximum change can be +-65535 since the value type is 16 bit wide.
	 * The remaining 15 bits are used for fractional delta value:
	 * 1 value difference can take at most 32768 steps, which is equivalent
	 * to 2^31 steps over the whole 16bit range.
	 */
	template< typename Type >
	struct Computations <Type, uint16_t>
	{
		uint32_t accumulatedValue;
		int32_t deltaValue;

		Computations()
		:	accumulatedValue(0), deltaValue(0)
		{
		}

		void inline
		initialize(Type begin, Type end, uint32_t steps)
		{
			int32_t delta = (static_cast<int32_t>(end) - begin) << 15;
			deltaValue = delta / static_cast<int32_t>(steps);
			if (deltaValue == 0)
				deltaValue = delta > 0 ? 1 : -1;
			accumulatedValue = (static_cast<uint32_t>(begin) << 15) + deltaValue / 2;
		}

		void inline
		step()
		{
			accumulatedValue += deltaValue;
		}

		Type inline
		get()
		{
			return static_cast<Type>(accumulatedValue >> 15);
		}
	};

	// On the AVR 64bit variables do not exist, therefore this must be excluded,
	// so that it may revert back to using floats.
	// It's not pretty, but neither is using uint32_t on an 8bit CPU to begin with.
#if !defined(XPCC_CPU_AVR)

	/**
	 * uint32_t implementation using signed 32.16 fixed point arithmetic.
	 * The maximum change can be +-2^32 since the value type is 32 bit wide.
	 * The remaining 16 bits are used for fractional delta steps:
	 * 1 value difference can take at most 65356 steps, which is equivalent
	 * to 2^63 steps over the whole range.
	 * Note: we use 16 bits for the fractionals here, so that it is byte-aligned.
	 * This should allow CPUs without a barrelshifter to copy the value instead of shifting it.
	 */
	template< typename Type >
	struct Computations <Type, uint32_t>
	{
		uint64_t accumulatedValue;
		int64_t deltaValue;

		Computations()
		:	accumulatedValue(0), deltaValue(0)
		{
		}

		void inline
		initialize(Type begin, Type end, uint32_t steps)
		{
			int64_t delta = (static_cast<int64_t>(end) - begin) << 16;
			deltaValue = delta / static_cast<int32_t>(steps);
			if (deltaValue == 0)
				deltaValue = delta > 0 ? 1 : -1;
			accumulatedValue = (static_cast<uint64_t>(begin) << 16) + deltaValue / 2;
		}

		void inline
		step()
		{
			accumulatedValue += deltaValue;
		}

		Type inline
		get()
		{
			return static_cast<Type>(accumulatedValue >> 16);
		}
	};
#endif

	// create an instance of the calculation helper
	Computations<T, UnsignedType> computations;

public:
	FastRamp()
	:	computations()
	{
	}

	/**
	 * This method calculates the incline of the ramp.
	 * @param	begin	the beginning of the ramp
	 * @param	end		the end of the ramp
	 * @param	steps	the number of steps for the ramp.
	 */
	void xpcc_always_inline
	initialize(T begin, T end, StepType steps)
	{
		computations.initialize(begin, end, steps);
	}

	/// update the intermediate value for one step
	void xpcc_always_inline
	step()
	{
		computations.step();
	}

	/// @return the intermediate value.
	T xpcc_always_inline
	getValue()
	{
		return computations.get();
	}

	/// stops the interpolation.
	void xpcc_always_inline
	stop()
	{
		computations.deltaValue = 0;
	}
};

}	// namespace ui

}	// namespace xpcc

#endif	// XPCC_UI_LINEAR_INTERPOLATION_HPP
