/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TEST_MOCK_IODEVICE_HPP
#define MODM_TEST_MOCK_IODEVICE_HPP

#include <modm/io/iodevice.hpp>
#include <cstring>  // memset

namespace modm_test::platform
{

// ----------------------------------------------------------------------------
// simple IODevice which stores all data in a memory buffer
// used for testing the output of an IOStream

class IODevice : public modm::IODevice
{
public:
	inline IODevice() :
		bytesWritten(0) {}

	/// Write a single char to the buffer.
	inline virtual void
	write(char c)
	{
		this->buffer[this->bytesWritten] = c;
		this->bytesWritten++;
	}

	using modm::IODevice::write;

	inline virtual void
	flush()
	{
		clear();
	}

	/// Reading is not implemented
	inline virtual bool
	read(char& /*c*/)
	{
		return false;
	}

	/// Clear the buffer and reset counter.
	inline void
	clear()
	{
		memset(this->buffer, 0, this->buffer_length);
		this->bytesWritten = 0;
	}

	static constexpr std::size_t buffer_length = 100;
	char buffer[buffer_length];
	size_t bytesWritten;
};

} // modm_test::platform namespace

#endif // MODM_TEST_MOCK_IODEVICE_HPP
