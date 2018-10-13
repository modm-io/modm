/*
 * Copyright (c) 2010-2012, Fabian Greif
 * Copyright (c) 2010, 2012, Martin Rosekeit
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/operator.hpp>

#include "vector2.hpp"

// this explicit namespace is needed here, otherwise we get an error about
// "specialization of ... in different namespace"
namespace modm
{
	template<>
	int16_t
	Vector<int16_t, 2>::getLength() const
	{
		int32_t t;

		t = math::mul(this->x, this->x);
		t = math::mac(t, this-> y, this->y);

		return math::sqrt(t);
	}

	template<>
	int32_t
	Vector<int16_t, 2>::getLengthSquared() const
	{
		int32_t t;

		t = math::mul(this->x, this->x);
		t = math::mac(t,this-> y, this->y);

		return t;
	}

	template<>
	int32_t
	Vector<int16_t, 2>::dot(const modm::Vector<int16_t, 2>& other) const
	{
		int32_t t;

		t = math::mul(this->x, other.x);
		t = math::mac(t,this->y, other.y);

		return t;
	}

	// ------------------------------------------------------------------------
	template<>
	template<>
	Vector<double, 2>
	Vector<float, 2>::convert() const
	{
		return Vector<double, 2>(this->x, this->y);
	}

	template<>
	template<>
	Vector<float, 2>
	Vector<double, 2>::convert() const
	{
		return Vector<float, 2>(this->x, this->y);
	}
}
