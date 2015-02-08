// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_TIMESTAMP_HPP
#define	XPCC_TIMESTAMP_HPP

#include <stdint.h>

#include <xpcc/io/iostream.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{

/**
 * Simple timestamp
 *
 * The timestamp is calculated in milliseconds.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	processing
 */
template< typename T >
class TimestampBase
{
public:
	typedef T Type;
	typedef typename xpcc::ArithmeticTraits<T>::SignedType SignedType;

public:
	/// @param time in ms
	TimestampBase(const Type time = 0) :
		time(time)
	{
	}

	TimestampBase(const TimestampBase<T> &other) :
		time(other.time)
	{
	}

	inline Type
	getTime() const
	{
		return time;
	}

	inline TimestampBase<T>
	operator + (const TimestampBase<T>& other) const
	{
		return TimestampBase<T>(time + other.time);
	}

	inline TimestampBase<T>
	operator - (const TimestampBase<T>& other) const
	{
		return TimestampBase<T>(time - other.time);
	}

	inline bool
	operator == (const TimestampBase<T>& other) const
	{
		return (time == other.time);
	}

	inline bool
	operator != (const TimestampBase<T>& other) const
	{
		return (time != other.time);
	}

	inline bool
	operator < (const TimestampBase<T>& other) const
	{
		return SignedType(time - other.time) < 0;
	}

	inline bool
	operator > (const TimestampBase<T>& other) const
	{
		return SignedType(time - other.time) > 0;
	}

	inline bool
	operator <= (const TimestampBase<T>& other) const
	{
		return SignedType(time - other.time) <= 0;
	}

	inline bool
	operator >= (const TimestampBase<T>& other) const
	{
		return SignedType(time - other.time) >= 0;
	}

private:
	Type time;
};

typedef TimestampBase<uint_fast16_t> Timestamp;
typedef TimestampBase<uint32_t> LongTimestamp;

// ------------------------------------------------------------------------
template< typename T >
inline IOStream&
operator << (IOStream& os, const TimestampBase<T>& t)
{
	os << t.getTime();
	return os;
}

}	// namespace xpcc

#endif	// XPCC_TIMESTAMP_HPP
