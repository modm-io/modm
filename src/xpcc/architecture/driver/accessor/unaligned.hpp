// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ACCESSOR_UNALIGNED_HPP
#define XPCC_ACCESSOR_UNALIGNED_HPP

#include <xpcc/architecture/detect.hpp>
#include <xpcc/architecture/utils.hpp>

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
#ifdef XPCC__CPU_CORTEX_M0
	{ write(value); }
#else
	: data(value) {}
#endif


	inline operator T() const
	{
#ifdef XPCC__CPU_CORTEX_M0
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
#ifdef XPCC__CPU_CORTEX_M0
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
#ifdef XPCC__CPU_CORTEX_M0
		// memcpy((uint8_t*)&value, data, sizeof(T));
		// manual copy is faster for small sizes
		for(uint_fast8_t ii=0; ii<sizeof(T); ii++)
			reinterpret_cast<uint8_t*>(&value)[ii] = data[ii];
#else
		value = data;
#endif
	}


protected:
#ifdef XPCC__CPU_CORTEX_M0
	uint8_t data[sizeof(T)];
#else
	T data;
#endif
} ATTRIBUTE_PACKED;

/**
 * Accesses a memory location using a unaligned-safe method.
 *
 * @ingroup	accessor
 */
template< typename T, typename U>
ALWAYS_INLINE unaligned_t<T>*
asUnaligned(U* value)
{
	return reinterpret_cast< unaligned_t<T>* >(value);
}

}

#endif	// XPCC_ACCESSOR_UNALIGNED_HPP
