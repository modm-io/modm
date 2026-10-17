/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GEOMETRIC_TRAITS_HPP
#define MODM_GEOMETRIC_TRAITS_HPP

#include <cmath>
#include <stdint.h>
#include <modm/architecture/utils.hpp>

namespace modm
{
	/**
	 * \brief	Traits for all geometric classes
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Fabian Greif
	 */
	template <typename T>
	struct GeometricTraits
	{
		static constexpr bool isValidType = false;

		/**
		 * \brief	Round if converting from a floating point base to
		 * 			a integer base.
		 *
		 * For T = \c float and \c double this method is specialized to return
		 * the result directly without any rounding.
		 */
		static constexpr T
		round(float value)
		{
			return ::round(value);
		}
	};

	template <>
	struct GeometricTraits<int8_t>
	{
		static constexpr bool isValidType = true;

		using FloatType = float;
		using WideType = int16_t;

		static constexpr int8_t
		round(float value)
		{
			return ::round(value);
		}
	};

	// TODO is this useful?
	template <>
	struct GeometricTraits<uint8_t>
	{
		static constexpr bool isValidType = true;

		using FloatType = float;
		using WideType = int16_t;

		static constexpr uint8_t
		round(float value)
		{
			return ::round(value);
		}
	};

	template <>
	struct GeometricTraits<int16_t>
	{
		static constexpr bool isValidType = true;

		using FloatType = float;
		using WideType = int32_t;

		static constexpr int16_t
		round(float value)
		{
			return ::round(value);
		}
	};

	template <>
	struct GeometricTraits<int32_t>
	{
		static constexpr bool isValidType = true;

		using FloatType = float;

		// Usually the range of a int32_t is big enough so that no
		// conversion to int64_t is required. This exception is made because
		// 64-bit operations are very, very slow on an AVR.
		using WideType = int32_t;

		static constexpr int32_t
		round(float value)
		{
			return ::round(value);
		}
	};

	template <>
	struct GeometricTraits<float>
	{
		static constexpr bool isValidType = true;

		using FloatType = float;
		using WideType = float;

		static constexpr float
		round(float value)
		{
			return value;
		}
	};

	template <>
	struct GeometricTraits<double>
	{
		static constexpr bool isValidType = true;

		using FloatType = double;
		using WideType = double;

		static constexpr double
		round(double value)
		{
			return value;
		}
	};
}

#endif // MODM_GEOMETRIC_TRAITS_HPP
