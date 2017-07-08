/*
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PLATFORM_ACCESSOR_UNALIGNED_HPP
#define MODM_PLATFORM_ACCESSOR_UNALIGNED_HPP

#include <modm/architecture/utils.hpp>

namespace modm
{

template< typename T >
struct
unaligned_t
{
	inline unaligned_t() : data{0} {}

	inline unaligned_t(T value)
	: data(value) {}

	inline operator T() const
	{
		return data;
	}

protected:
	void inline
	write(T &value)
	{
		data = value;
	}

	void inline
	read(T &value) const
	{
		value = data;
	}

protected:
	T data;
} modm_packed;

}	// namespace modm

#endif	// MODM_PLATFORM_ACCESSOR_UNALIGNED_HPP
