/*
 * Copyright (c) 2026, Niklas Hauser
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

template <size_t SIZE>
class UartRxBuffer : public modm::Uart::RxBuffer, public modm::atomic::Queue<uint8_t, SIZE> {};
template <size_t SIZE>
class UartTxBuffer : public modm::Uart::TxBuffer, public modm::atomic::Queue<uint8_t, SIZE> {};

template< class Hal, class... Buffers >
class BufferedUart;

template<size_t SIZE, class Hal, class... Buffers>
class BufferedUart<Hal, UartTxBuffer<SIZE>, Buffers...>: public BufferedUart<Hal, Buffers...>
{
	template< class Hal_, class... Buffers_> friend class BufferedUart;
	using Parent = BufferedUart<Hal, Buffers...>;
	static_assert(not Parent::TxBufferSize, "BufferedUart accepts at most one TxBuffer type");
	static inline UartTxBuffer<SIZE> txBuffer;
	static inline uint8_t dmaTxBuffer[SIZE];
	static inline std::size_t dmaTxLength = 0;

	static inline void
	serviceDmaTx()
	{
		if ((dmaTxLength != 0) and Hal::completedWriteBuffer()) {
			dmaTxLength = 0;
		}
	}

	static inline bool
	startDmaBurst()
	{
		if ((dmaTxLength != 0) or txBuffer.isEmpty()) {
			return false;
		}

		std::size_t count = 0;
		while ((count < SIZE) and (not txBuffer.isEmpty()))
		{
			dmaTxBuffer[count++] = txBuffer.get();
			txBuffer.pop();
		}

		if ((count != 0) and Hal::startWriteBuffer(dmaTxBuffer, count)) {
			dmaTxLength = count;
			return true;
		}

		return false;
	}

	static bool
	InterruptCallback(bool first)
	{
		atomic::Lock lock;
		if constexpr (Parent::RxBufferSize) Parent::InterruptCallback(false);
		serviceDmaTx();
		if (dmaTxLength == 0) {
			if (not startDmaBurst()) {
				Hal::disableInterrupt(Hal::Interrupt::TxEmpty);
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
		Hal::enableInterrupt(Hal::Interrupt::TxEmpty);
		startDmaBurst();
	}

	static bool
	write(uint8_t data)
	{
		atomic::Lock lock;
		serviceDmaTx();

		if ((dmaTxLength == 0) and txBuffer.isEmpty())
		{
			dmaTxBuffer[0] = data;
			if (Hal::startWriteBuffer(dmaTxBuffer, 1)) {
				dmaTxLength = 1;
				return true;
			}
		}

		if (not txBuffer.push(data)) {
			return false;
		}

		Hal::enableInterrupt(Hal::Interrupt::TxEmpty);
		if (dmaTxLength == 0) {
			startDmaBurst();
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

	static void flushWriteBuffer() { modm::this_fiber::poll([&]{ return isWriteFinished(); }); }
	static bool
	isWriteFinished()
	{
		atomic::Lock lock;
		serviceDmaTx();
		return txBuffer.isEmpty() and (dmaTxLength == 0) and Hal::isTransmitRegisterEmpty();
	}

	static std::size_t
	transmitBufferSize()
	{
		atomic::Lock lock;
		serviceDmaTx();
		return txBuffer.getSize() + dmaTxLength;
	}

	static std::size_t
	discardTransmitBuffer()
	{
		atomic::Lock lock;
		serviceDmaTx();
		std::size_t count = txBuffer.getSize();
		if (dmaTxLength != 0) {
			count += dmaTxLength;
			dmaTxLength = 0;
			Hal::setTransmitterEnable(false);
		}

		while(not txBuffer.isEmpty())
		{
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
	read(uint8_t &data)
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

	static std::size_t receiveBufferSize() { return rxBuffer.getSize(); }

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

} // namespace modm::platform
