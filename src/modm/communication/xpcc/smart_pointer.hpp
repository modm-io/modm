/*
 * Copyright (c) 2009, Georgi Grinshpun
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

#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <modm/io/iostream.hpp>

namespace xpcc
{

/**
 * Reference counted payload buffer.
 *
 * The payload is word aligned and at least one byte large, so that
 * `getPointer()` always returns a valid address.
 *
 * @ingroup	modm_communication_xpcc
 */
class SmartPointer
{
public:
	/// Empty payload
	SmartPointer() : SmartPointer(uint16_t(0)) {}

	/// Uninitialized payload of `size` bytes
	explicit SmartPointer(uint16_t size)
	:	data(std::make_shared_for_overwrite<uint32_t[]>(size / 4 + 1)), size(size)
	{}

	// Must use a pointer to T here, otherwise the compiler can't distinguish
	// between constructor and copy constructor!
	template<typename T>
	explicit SmartPointer(const T *payload)
	:	SmartPointer(uint16_t(sizeof(T)))
	{
		std::memcpy(getPointer(), payload, sizeof(T));
	}

	const uint8_t *
	getPointer() const
	{ return reinterpret_cast<const uint8_t *>(data.get()); }

	uint8_t *
	getPointer()
	{ return reinterpret_cast<uint8_t *>(data.get()); }

	uint16_t
	getSize() const
	{ return size; }

	template<typename T>
	const T&
	get() const
	{ return *reinterpret_cast<const T*>(data.get()); }

	template<typename T>
	bool
	get(T& value) const
	{
		if (sizeof(T) != size) return false;
		std::memcpy(&value, getPointer(), sizeof(T));
		return true;
	}

	bool
	operator==(const SmartPointer& other) const
	{ return data == other.data; }

private:
	std::shared_ptr<uint32_t[]> data;
	uint16_t size;

	friend modm::IOStream&
	operator<<(modm::IOStream& s, const SmartPointer& v)
	{
		s << "0x" << modm::hex;
		for (uint16_t ii = 0; ii < v.size; ii++) s << v.getPointer()[ii];
		return s << modm::ascii;
	}
};

}	// namespace xpcc

namespace modm
{

using SmartPointer [[deprecated("Use xpcc::SmartPointer instead")]] = xpcc::SmartPointer; // DEPRECATED: 2027q4

}	// namespace modm
