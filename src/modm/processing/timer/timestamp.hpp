/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2010-2011, Martin Rosekeit
 * Copyright (c) 2012-2013, Sascha Schade
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_TIMESTAMP_HPP
#define	MODM_TIMESTAMP_HPP

#include <stdint.h>

#include <modm/io/iostream.hpp>
#include <modm/utils/arithmetic_traits.hpp>

namespace modm
{

/**
 * Generic timestamp for 16bit and 32bit timestamps of variable timebase.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	software_timer
 */
template< typename T >
class GenericTimestamp
{
public:
	typedef T Type;
	typedef typename modm::ArithmeticTraits<T>::SignedType SignedType;

public:
	/// @param time in ms
	GenericTimestamp(const Type time = 0) :
		time(time)
	{
	}

	GenericTimestamp(const GenericTimestamp<T> &other) :
		time(other.time)
	{
	}

	inline Type
	getTime() const
	{
		return time;
	}

	inline GenericTimestamp<T>
	operator + (const GenericTimestamp<T>& other) const
	{
		return GenericTimestamp<T>(time + other.time);
	}

	inline GenericTimestamp<T>
	operator - (const GenericTimestamp<T>& other) const
	{
		return GenericTimestamp<T>(time - other.time);
	}

	inline bool
	operator == (const GenericTimestamp<T>& other) const
	{
		return (time == other.time);
	}

	inline bool
	operator != (const GenericTimestamp<T>& other) const
	{
		return (time != other.time);
	}

	inline bool
	operator < (const GenericTimestamp<T>& other) const
	{
		return SignedType(time - other.time) < 0;
	}

	inline bool
	operator > (const GenericTimestamp<T>& other) const
	{
		return SignedType(time - other.time) > 0;
	}

	inline bool
	operator <= (const GenericTimestamp<T>& other) const
	{
		return SignedType(time - other.time) <= 0;
	}

	inline bool
	operator >= (const GenericTimestamp<T>& other) const
	{
		return SignedType(time - other.time) >= 0;
	}

private:
	Type time;
};

/// 16bit timestamp, which can hold up to 65 seconds at millisecond resolution.
/// @ingroup	software_timer
using ShortTimestamp = GenericTimestamp<uint16_t>;

/// 32bit timestamp, which can hold up to 49 days at millisecond resolution.
/// @ingroup	software_timer
using Timestamp      = GenericTimestamp<uint32_t>;

// ------------------------------------------------------------------------
template< typename T >
inline IOStream&
operator << (IOStream& os, const GenericTimestamp<T>& t)
{
	os << t.getTime();
	return os;
}

}	// namespace modm

#endif	// MODM_TIMESTAMP_HPP
