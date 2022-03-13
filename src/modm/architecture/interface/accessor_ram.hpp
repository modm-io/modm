/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_INTERFACE_ACCESSOR_RAM_HPP
#define	MODM_INTERFACE_ACCESSOR_RAM_HPP

#include <cstddef>
#include <modm/architecture/utils.hpp>

namespace modm
{

namespace accessor
{

/**
 * Pointer to RAM
 *
 * @ingroup modm_architecture_accessor
 * @author	Fabian Greif
 */
template<typename T>
class Ram
{
public:
	Ram(const T* addr = 0) :
		address(addr)
	{
	}

	template <typename U>
	explicit Ram(const Ram<U>& rhs) :
		address((T*) rhs.address)
	{
	}

	const T
	operator *() const
	{
		return *address;
	}

	const T
	operator [](std::size_t index) const
	{
		return *(address + index);
	}

	Ram&
	operator ++ ()
	{
		*this += 1;
		return *this;
	}

	Ram
	operator ++ (int)
	{
		Ram ret = *this;
		++*this;
		return ret;
	}

	Ram&
	operator -- ()
	{
		*this -= 1;
		return *this;
	}

	Ram&
	operator -- (int)
	{
		Ram ret = *this;
		--*this;
		return ret;
	}

	Ram&
	operator += (std::size_t rhs)
	{
		address += rhs;
		return *this;
	}

	Ram&
	operator -= (std::size_t rhs)
	{
		address -= rhs;
		return *this;
	}

	const T*
	getPointer() const
	{
		return address;
	}

private:
	const T* address;
};

}	// namespace accessor

}	// namespace modm

#endif	// MODM_INTERFACE_ACCESSOR_RAM_HPP
