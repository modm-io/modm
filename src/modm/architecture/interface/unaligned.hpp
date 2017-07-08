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

#ifndef MODM_INTERFACE_UNALIGNED_ACCESS_HPP
#define MODM_INTERFACE_UNALIGNED_ACCESS_HPP

#ifndef __DOXYGEN__
// These functions may be implemented as header files, so we cannot rely on
// there being a link-able function and delegate this choice to the platform.
#include <modm/platform/core/unaligned.hpp>
#endif

namespace modm
{

#ifdef __DOXYGEN__
/**
 * Accesses a type using byte-wise copy.
 *
 * This wrapper manages unaligned access to memory, by copying
 * the memory to and from the stack, which is always correctly aligned.
 * Use this with teh `asUnaligned` helper:
 * @code
 * uint8_t *buffer;
 * // `u32` is a pointer to a unaligned_t<uint32_t> !
 * auto *u32 = modm::asUnaligned<uint32_t>(buffer);
 * // write to the unaligned location
 * *u32 = input;
 * // read from the unaligned location
 * output = *u32;
 * @endcode
 *
 * @ingroup	interface
 * @author	Niklas Hauser
 */
template< typename T >
struct
unaligned_t
{
	unaligned_t();
	unaligned_t(T value);

	inline operator T() const;

protected:
	void inline
	write(T &value);

	void inline
	read(T &value) const;
};
#endif

/**
 * Accesses a memory location using a unaligned-safe method.
 *
 * @ingroup	interface
 */
template< typename T, typename U>
modm_always_inline unaligned_t<T>*
asUnaligned(U* value)
{
	return reinterpret_cast< unaligned_t<T>* >(value);
}

}	// namespace modm

#endif	// MODM_INTERFACE_UNALIGNED_ACCESS_HPP
