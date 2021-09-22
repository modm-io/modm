/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <limits>
#include <concepts>

// TODO I'm not sure about this :/

namespace modm {
	template<typename T>
	concept unsigned_integral_max8 = std::unsigned_integral<T> and std::numeric_limits<T>::digits <= 8;
	
	template<typename T>
	concept unsigned_integral_max16 = std::unsigned_integral<T> and std::numeric_limits<T>::digits <= 16;

	template<typename T>
	concept unsigned_integral_max32 = std::unsigned_integral<T> and std::numeric_limits<T>::digits <= 32;

	template<typename T>
	concept unsigned_integral_max64 = std::unsigned_integral<T> and std::numeric_limits<T>::digits <= 64;
}