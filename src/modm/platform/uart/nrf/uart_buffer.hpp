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
	static inline uint8_t txBurstBuffer[SIZE];
	static inline volatile std::size_t txBurstLength = 0;

	static inline void
	serviceTxBurst()
	{
		if ((txBurstLength != 0) and Hal::completedWriteBuffer()) {
			txBurstLength = 0;
		}
	}

	static inline bool
	startTxBurst()
	{
		if ((txBurstLength != 0) or txBuffer.isEmpty()) {
			return false;
		}

		std::size_t count = 0;
		while ((count < SIZE) and (not txBuffer.isEmpty()))
		{
			txBurstBuffer[count++] = txBuffer.get();
			txBuffer.pop();
		}

		if ((count != 0) and Hal::startWriteBuffer(txBurstBuffer, count)) {
			txBurstLength = count;
			return true;
		}

		return false;
	}

	static bool
	InterruptCallback(bool first)
	{
		atomic::Lock lock;
		if constexpr (Parent::RxBufferSize) Parent::InterruptCallback(false);
		serviceTxBurst();
		if (txBurstLength == 0) {
			if (not startTxBurst()) {
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
		startTxBurst();
	}

	static bool
	write(uint8_t data)
	{
		atomic::Lock lock;
		serviceTxBurst();

		if ((txBurstLength == 0) and txBuffer.isEmpty())
		{
			txBurstBuffer[0] = data;
			if (Hal::startWriteBuffer(txBurstBuffer, 1)) {
				txBurstLength = 1;
				return true;
			}
		}

		if (not txBuffer.push(data)) {
			return false;
		}

		Hal::enableInterrupt(Hal::Interrupt::TxEmpty);
		if (txBurstLength == 0) {
			startTxBurst();
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
		serviceTxBurst();
		return txBuffer.isEmpty() and (txBurstLength == 0) and Hal::isTransmitRegisterEmpty();
	}

	static std::size_t
	transmitBufferSize()
	{
		atomic::Lock lock;
		serviceTxBurst();
		return txBuffer.getSize() + txBurstLength;
	}

	static std::size_t
	discardTransmitBuffer()
	{
		atomic::Lock lock;
		serviceTxBurst();
		std::size_t count = txBuffer.getSize();
		if (txBurstLength != 0) {
			count += txBurstLength;
			txBurstLength = 0;
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
