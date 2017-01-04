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

#ifndef XPCC_ACCESSOR_UNALIGNED_HPP
#define XPCC_ACCESSOR_UNALIGNED_HPP

#include <modm/architecture/detect.hpp>
#include <modm/architecture/utils.hpp>

namespace xpcc
{

/**
 * Accesses a type using byte-wise copy.
 *
 * This wrapper manages unaligned access to memory, by copying
 * the memory to and from the stack, which is always correctly aligned.
 * Use this with teh `asUnaligned` helper:
 * @code
 * uint8_t *buffer;
 * // `u32` is a pointer to a unaligned_t<uint32_t> !
 * auto *u32 = xpcc::asUnaligned<uint32_t>(buffer);
 * // write to the unaligned location
 * *u32 = input;
 * // read from the unaligned location
 * output = *u32;
 * @endcode
 *
 * @ingroup	accessor
 * @author	Niklas Hauser
 */
template< typename T >
struct
unaligned_t
{
	inline unaligned_t() : data{0} {}

	inline unaligned_t(T value)
#ifdef XPCC_CPU_CORTEX_M0
	{ write(value); }
#else
	: data(value) {}
#endif


	inline operator T() const
	{
#ifdef XPCC_CPU_CORTEX_M0
		T t;
		read(t);
		return t;
#else
		return data;
#endif
	}

protected:
	void inline
	write(T &value)
	{
#ifdef XPCC_CPU_CORTEX_M0
		// memcpy(data, (uint8_t*)&value, sizeof(T));
		// manual copy is faster for small sizes
		for(uint_fast8_t ii=0; ii<sizeof(T); ii++)
			data[ii] = reinterpret_cast<uint8_t*>(&value)[ii];
#else
		data = value;
#endif
	}

	void inline
	read(T &value) const
	{
#ifdef XPCC_CPU_CORTEX_M0
		// memcpy((uint8_t*)&value, data, sizeof(T));
		// manual copy is faster for small sizes
		for(uint_fast8_t ii=0; ii<sizeof(T); ii++)
			reinterpret_cast<uint8_t*>(&value)[ii] = data[ii];
#else
		value = data;
#endif
	}


protected:
#ifdef XPCC_CPU_CORTEX_M0
	uint8_t data[sizeof(T)];
#else
	T data;
#endif
} xpcc_packed;

/**
 * Accesses a memory location using a unaligned-safe method.
 *
 * @ingroup	accessor
 */
template< typename T, typename U>
xpcc_always_inline unaligned_t<T>*
asUnaligned(U* value)
{
	return reinterpret_cast< unaligned_t<T>* >(value);
}

}

#endif	// XPCC_ACCESSOR_UNALIGNED_HPP
