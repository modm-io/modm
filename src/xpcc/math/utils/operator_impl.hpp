/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
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

#ifndef	MODM_MATH_OPERATOR_HPP
	#error	"Don't include this file directly, use 'operator.hpp' instead!"
#endif

inline uint32_t
modm::math::mul(uint16_t a, uint16_t b)
{
	return static_cast<uint32_t>(a) * static_cast<uint32_t>(b);
}

inline int32_t
modm::math::mul(int16_t a, int16_t b)
{
	return static_cast<int32_t>(a) * static_cast<int32_t>(b);
}

inline int32_t
modm::math::mac(int32_t result, int16_t a, int16_t b)
{
	return result + static_cast<int32_t>(a) * static_cast<int32_t>(b);
}
