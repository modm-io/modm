/*
 * Copyright (c) 2021, 2025, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <modm/architecture/interface/uart.hpp>
#include <modm/processing/fiber.hpp>
#include <SEGGER_RTT.h>

namespace modm::platform
{

/**
 * Real Time Transfer (RTT) Uart Interface
 *
 * @author		Niklas Hauser
 * @ingroup		modm_platform_rtt
 */
template<uint8_t Channel>
class Rtt : public ::modm::Uart
{
	static_assert(Channel < SEGGER_RTT_MAX_NUM_UP_BUFFERS,
				  "Channel index too large for RTT configuration!");
public:
	inline void
	static writeBlocking(uint8_t data)
	{
		modm::this_fiber::poll([&]{ return write(data); });
	}

	inline void
	static writeBlocking(const uint8_t *data, std::size_t length)
	{
		while (true)
		{
			const std::size_t written = write(data, length);
			length -= written;
			if (length == 0) return;
			data += written;
			modm::this_fiber::yield();
		}
	}

	inline void
	static flushWriteBuffer()
	{
		modm::this_fiber::poll([&]{ return isWriteFinished(); });
	}

	inline bool
	static write(uint8_t data)
	{
		return SEGGER_RTT_PutCharSkipNoLock(Channel, data);
	}

	inline std::size_t
	static write(const uint8_t *data, std::size_t length)
	{
		if (length == 0) return 0;
		if (not SEGGER_RTT_WriteSkipNoLock(Channel, data, length))
		{
			const std::size_t available = std::min<size_t>(SEGGER_RTT_GetAvailWriteSpace(Channel), length);
			if (available and SEGGER_RTT_WriteSkipNoLock(Channel, data, available))
				return available;
		}
		return 0;
	}

	inline bool
	static isWriteFinished()
	{
		return SEGGER_RTT_HASDATA_UP(Channel) == 0;
	}

	std::size_t
	static transmitBufferSize()
	{
		return SEGGER_RTT_GetBytesInBuffer(Channel);
	}

	inline std::size_t
	static discardTransmitBuffer()
	{
		// cannot do it safely due to race conditions with the debugger
		return 0;
	}

	inline bool
	static read(uint8_t &data)
	{
		return read(&data, 1) == 1;
	}

	std::size_t
	static read(uint8_t *data, std::size_t length)
	{
		return SEGGER_RTT_ReadNoLock(Channel, data, length);
	}

	inline std::size_t
	static receiveBufferSize()
	{
		return SEGGER_RTT_GetBytesInDownBuffer(Channel);
	}

	inline std::size_t
	static discardReceiveBuffer()
	{
		uint8_t buffer[32];
		std::size_t length{0};
		while (size_t size = read(buffer, 32)) length += size;
		return length;
	}

	inline bool
	static hasError()
	{ return false; }

	inline void
	static clearError() {}
};

}	// namespace modm::platform
