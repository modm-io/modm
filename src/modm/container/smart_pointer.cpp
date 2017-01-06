/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "smart_pointer.hpp"

// ----------------------------------------------------------------------------
// must allocate at least five bytes, so getPointer() does return
// a valid address
modm::SmartPointer::SmartPointer() :
	ptr(new uint8_t[5])
{
	ptr[0] = 1;
	ptr[2] = 0;
	ptr[3] = 0;
}

modm::SmartPointer::SmartPointer(const SmartPointer& other) :
	ptr(other.ptr)
{
	ptr[0]++;
}

// must allocate at least five bytes, so getPointer() does return
// a valid address
modm::SmartPointer::SmartPointer(uint16_t size) :
	ptr(new uint8_t[size ? size + 4 : 5])
{
	ptr[0] = 1;
	*reinterpret_cast<uint16_t*>(ptr + 2) = size;
}

modm::SmartPointer::~SmartPointer()
{
	if (--ptr[0] == 0) {
		delete[] ptr;
	}
}

// ----------------------------------------------------------------------------
bool
modm::SmartPointer::operator == (const SmartPointer& other)
{
	return (this->ptr == other.ptr);
}

modm::SmartPointer&
modm::SmartPointer::operator = (const SmartPointer& other)
{
	if (--ptr[0] == 0) {
		delete[] ptr;
	}

	ptr = other.ptr;
	ptr[0]++;

	return *this;
}

// ----------------------------------------------------------------------------
modm::IOStream&
modm::operator << (modm::IOStream& s, const modm::SmartPointer& v)
{
	s << "0x" << modm::hex;
	for (uint8_t i = 4; i < v.getSize() + 4; i++)
	{
		s << v.ptr[i];
	}
	s << modm::ascii;
	return s;
}
