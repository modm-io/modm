/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011, 2013-2017, 2024, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, 2016, Kevin Läufer
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2024, Dima Barsky
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/driver/atomic/queue.hpp>
#include <modm/architecture/interface/uart.hpp>
#include "uart_base.hpp"

namespace modm::platform
{

/**
 * Universal asynchronous receiver transmitter (implementation based on modm::atomic::Queue)
 *
 * @author      Kevin Laeufer
 * @author      Niklas Hauser
 * @author      Dima Barsky
 * @ingroup     modm_platform_uart
 * @{
 */
template <size_t SIZE>
class UartRxBuffer : public modm::Uart::RxBuffer, public modm::atomic::Queue<uint8_t, SIZE> {};
template <size_t SIZE>
class UartTxBuffer : public modm::Uart::TxBuffer, public modm::atomic::Queue<uint8_t, SIZE> {};
/// @}

/// @cond
template< class Hal, class... Buffers >
class BufferedUart;

template<size_t SIZE, class Hal, class... Buffers>
class BufferedUart<Hal, UartTxBuffer<SIZE>, Buffers...>: public BufferedUart<Hal, Buffers...>
{
	template< class Hal_, class... Buffers_> friend class BufferedUart;
	using Parent = BufferedUart<Hal, Buffers...>;
	static_assert(not Parent::TxBufferSize, "BufferedUart accepts at most one TxBuffer type");
	static inline UartTxBuffer<SIZE> txBuffer;

	static bool
	InterruptCallback(bool first)
	{
		if constexpr (Parent::RxBufferSize) Parent::InterruptCallback(false);

		if (Hal::isTransmitRegisterEmpty())
		{
			if (txBuffer.isEmpty())
			{
				atomic::Lock lock;
				// transmission finished, disable TxEmpty interrupt
				Hal::disableInterrupt(Hal::Interrupt::TxEmpty);
			}
			else
			{
				Hal::write(txBuffer.get());
				txBuffer.pop();
			}
		}
		if (first) Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::OverrunError);
		return true;
	}

public:
	static constexpr size_t TxBufferSize = SIZE;

	template< class SystemClock, baudrate_t baudrate, percent_t tolerance=pct(1) >
	static inline void
	initialize(Hal::Parity parity=Hal::Parity::Disabled, Hal::WordLength length=Hal::WordLength::Bit8)
	{
		Parent::template initialize<SystemClock, baudrate, tolerance>(parity, length);
		Hal::InterruptCallback = InterruptCallback;
	}

	static bool
	write(uint8_t data)
	{
		if (isWriteFinished()) Hal::write(data);
		else
		{
			if (not txBuffer.push(data)) return false;
			// Disable interrupts while enabling the transmit interrupt
			atomic::Lock lock;
			// Transmit Data Register Empty Interrupt Enable
			Hal::enableInterrupt(Hal::Interrupt::TxEmpty);
		}
		return true;
	}

	static std::size_t
	write(const uint8_t *data, std::size_t length)
	{
		std::size_t count{0};
		for (; count < length; ++count) if (not write(*data++)) break;
		return count;
	}

	static void
	flushWriteBuffer() { while(not isWriteFinished()); }

	static bool
	isWriteFinished() { return txBuffer.isEmpty() and Hal::isTransmitRegisterEmpty(); }

	static std::size_t
	transmitBufferSize() { return txBuffer.getSize(); }

	std::size_t
	discardTransmitBuffer()
	{
		{
			atomic::Lock lock;
			// disable interrupt since buffer will be cleared
			Hal::disableInterrupt(Hal::Interrupt::TxEmpty);
		}
		std::size_t count{0};
		while(not txBuffer.isEmpty())
		{
			++count;
			txBuffer.pop();
		}
		return count;
	}
};

template<size_t SIZE, class Hal, class... Buffers>
class BufferedUart<Hal, UartRxBuffer<SIZE>, Buffers...>: public BufferedUart<Hal, Buffers...>
{
	template< class Hal_, class... Buffers_> friend class BufferedUart;
	using Parent = BufferedUart<Hal, Buffers...>;
	static_assert(not Parent::RxBufferSize, "BufferedUart accepts at most one RxBuffer type");
	static inline UartRxBuffer<SIZE> rxBuffer;

	static bool
	InterruptCallback(bool first)
	{
		if (Hal::isReceiveRegisterNotEmpty())
		{
			uint8_t data;
			Hal::read(data);
			rxBuffer.push(data);
		}

		if constexpr (Parent::TxBufferSize) Parent::InterruptCallback(false);

		if (first) Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::OverrunError);
		return true;
	}

public:
	static constexpr size_t RxBufferSize = SIZE;

	template< class SystemClock, baudrate_t baudrate, percent_t tolerance=pct(1) >
	static inline void
	initialize(Hal::Parity parity=Hal::Parity::Disabled, Hal::WordLength length=Hal::WordLength::Bit8)
	{
		Parent::template initialize<SystemClock, baudrate, tolerance>(parity, length);
		Hal::InterruptCallback = InterruptCallback;
		Hal::enableInterrupt(Hal::Interrupt::RxNotEmpty);
	}

	static bool
	read(uint8_t &data, uint32_t /*ignored*/)
	{
		if (rxBuffer.isEmpty()) return false;

		data = rxBuffer.get();
		rxBuffer.pop();
		return true;
	}

	static std::size_t
	read(uint8_t *data, std::size_t length)
	{
		std::size_t count{0};
		for (; count < length; ++count)
		{
			if (rxBuffer.isEmpty()) break;

			*data++ = rxBuffer.get();
			rxBuffer.pop();
		}
		return count;
	}

	static std::size_t
	receiveBufferSize() { return rxBuffer.getSize(); }

	static std::size_t
	discardReceiveBuffer()
	{
		std::size_t count{0};
		while(not rxBuffer.isEmpty())
		{
			++count;
			rxBuffer.pop();
		}
		return count;
	}
};
/// @endcond

} // namespace modm::platform

