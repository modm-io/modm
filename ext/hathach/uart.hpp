/*
* Copyright (c) 2020, Erik Henriksson
* Copyright (c) 2020, Niklas Hauser
*
* This file is part of the modm project.
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/
// ----------------------------------------------------------------------------

#pragma once

#include "tusb.h"

#if defined(CFG_TUD_CDC) && CFG_TUD_CDC

#include <modm/architecture/interface/uart.hpp>

namespace modm::platform
{

template< uint8_t ITF=0 >
class UsbUart : public modm::Uart
{
	static_assert(ITF < CFG_TUD_CDC, "TinyUSB does not have this CDC port!");
public:
	static inline bool
	connected()
	{
		return tud_cdc_n_connected(ITF);
	}

	static inline void
	flushWriteBuffer()
	{
		tud_cdc_n_write_flush(ITF);
	}

	static inline bool
	write(uint8_t c)
	{
		bool rc = tud_cdc_n_write_char(ITF, c);
		tud_cdc_n_write_flush(ITF);
		return rc;
	}

	static inline std::size_t
	write(const uint8_t *data, std::size_t length)
	{
		std::size_t rc = tud_cdc_n_write(ITF, data, length);
		tud_cdc_n_write_flush(ITF);
		return rc;
	}

	static inline bool
	isWriteFinished()
	{
		return not transmitBufferSize();
	}

	static inline bool
	read(uint8_t& c)
	{
		if (receiveBufferSize())
			return tud_cdc_n_read(ITF, &c, sizeof(uint8_t));
		return false;
	}

	static inline std::size_t
	read(uint8_t *data, std::size_t length)
	{
		if (receiveBufferSize())
			return tud_cdc_n_read(ITF, data, length);
		return 0;
	}

	static inline std::size_t
	receiveBufferSize()
	{
		return tud_cdc_n_available(ITF);
	}

	static inline std::size_t
	discardReceiveBuffer()
	{
		const std::size_t rxsize = receiveBufferSize();
		tud_cdc_n_read_flush(ITF);
		return rxsize;
	}

	static inline std::size_t
	transmitBufferSize()
	{
		return tud_cdc_n_write_available(ITF);
	}

	static inline std::size_t
	discardTransmitBuffer()
	{
		const std::size_t txsize = transmitBufferSize();
		tud_cdc_n_write_clear(ITF);
		return txsize;
	}

	static inline bool
	hasError()
	{
		return false;
	}

	static inline void
	clearError()
	{}
};

using UsbUart0 = UsbUart<0>;
using UsbUart1 = UsbUart<1>;
using UsbUart2 = UsbUart<2>;
using UsbUart3 = UsbUart<3>;

} // namespace modm::platform

#endif
