/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <concepts>
#include <cmath>
#include <limits>
#include <modm/architecture/utils.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm
{
	/**
	 * \brief	Traits for all geometric classes
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Fabian Greif
	 * \author	Thomas Sommer
	 */
	template <typename T>
	struct GeometricTraits;

	template <std::integral T>
	struct GeometricTraits<T>
	{
		[[deprecated("Use an appropriate C++ concept instead!")]]
		static const bool isValidType = false;

		using FloatType = float;
		using WideType = modm::WideType<T>;
	};

	template <std::floating_point T>
	struct GeometricTraits<T>
	{
		[[deprecated("Use an appropriate C++ concept instead!")]]
		static const bool isValidType = true;

		using FloatType = T;
		using WideType = T;
	};

	#ifdef __AVR__
	template <>
	struct GeometricTraits<int32_t>
	{
		[[deprecated("Use an appropriate C++ concept instead!")]]
		static const bool isValidType = true;

		using FloatType = float;
		// conversion to int64_t is required. This exception is made because
		// 64-bit operations are very, very slow on an AVR.
		using WideType = int32_t;
	};
	#endif
}