/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011, 2013-2017, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, 2016, Kevin Läufer
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <modm/architecture/interface/uart.hpp>
#include "uart_base.hpp"

namespace modm::platform
{

/**
 * Universal asynchronous receiver transmitter (implementation based on FreeRTOS queues)
 *
 * @author      Kevin Laeufer
 * @author      Niklas Hauser
 * @author      Dima Barsky
 * @ingroup     modm_platform_uart
 * @{
 */
template <size_t SIZE>
class FreeRtosBuffer
{
	StaticQueue_t queueStructure;
	uint8_t storage[SIZE];
public:
	QueueHandle_t queue = xQueueCreateStatic(sizeof(storage), 1, storage, &queueStructure);
};

template <size_t SIZE>
class UartRxBufferFreeRtos : public modm::Uart::RxBuffer, public FreeRtosBuffer<SIZE>{};

template <size_t SIZE>
class UartTxBufferFreeRtos : public modm::Uart::TxBuffer, public FreeRtosBuffer<SIZE>{};
/// @}

/// @cond
template< class Hal, class... Buffers>
class BufferedUart;

template<size_t SIZE, class Hal, class... Buffers>
class BufferedUart<Hal, UartTxBufferFreeRtos<SIZE>, Buffers...>: public BufferedUart<Hal, Buffers...>
{
	template< class Hal_, class... Buffers_> friend class BufferedUart;
	using Parent = BufferedUart<Hal, Buffers...>;
	static_assert(not Parent::TxBufferSize, "BufferedUart accepts at most one TxBuffer type");
	static inline UartTxBufferFreeRtos<SIZE> txBuffer;

	static bool
	InterruptCallback(bool first)
	{
		BaseType_t xHigherPriorityTaskWoken{false};
		if constexpr (Parent::RxBufferSize)
			xHigherPriorityTaskWoken = Parent::InterruptCallback(false);

		if (Hal::isTransmitRegisterEmpty())
		{
			if (xQueueIsQueueEmptyFromISR(txBuffer.queue))
				// transmission finished, disable TxEmpty interrupt
				Hal::disableInterrupt(Hal::Interrupt::TxEmpty);
			else
			{
				uint8_t data;
				xQueueReceiveFromISR(txBuffer.queue, &data, &xHigherPriorityTaskWoken);
				Hal::write(data);
			}
		}
		if (first)
		{
			Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::OverrunError);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
		return xHigherPriorityTaskWoken;
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
		if (transmitBufferSize() == 0 and Hal::isTransmitRegisterEmpty())
			Hal::write(data);
		else
		{
			if (not xQueueSend(txBuffer.queue, &data, 0)) return false;
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
	writeBlocking(uint8_t data)
	{
		if (transmitBufferSize() == 0 and Hal::isTransmitRegisterEmpty())
			Hal::write(data);
		else
		{
			xQueueSend(txBuffer.queue, &data, portMAX_DELAY);
			// Disable interrupts while enabling the transmit interrupt
			atomic::Lock lock;
			// Transmit Data Register Empty Interrupt Enable
			Hal::enableInterrupt(Hal::Interrupt::TxEmpty);
		}
	}

	static void
	writeBlocking(const uint8_t *data, std::size_t length)
	{
		while (length-- != 0) writeBlocking(*data++);
	}

	static void
	flushWriteBuffer() { while(not isWriteFinished()); }

	static bool
	isWriteFinished() { return transmitBufferSize() == 0 and Hal::isTransmitRegisterEmpty(); }

	static std::size_t
	transmitBufferSize() { return uxQueueMessagesWaiting(txBuffer.queue); }

	std::size_t
	discardTransmitBuffer()
	{
		// disable interrupt since buffer will be cleared
		Hal::disableInterrupt(Hal::Interrupt::TxEmpty);
		std::size_t count = transmitBufferSize();
		xQueueReset(txBuffer.queue);
		return count;
	}
};

template<size_t SIZE, class Hal, class... Buffers>
class BufferedUart<Hal, UartRxBufferFreeRtos<SIZE>, Buffers...>: public BufferedUart<Hal, Buffers...>
{
	template< class Hal_, class... Buffers_> friend class BufferedUart;
	using Parent = BufferedUart<Hal, Buffers...>;
	static_assert(not Parent::RxBufferSize, "BufferedUart accepts at most one RxBuffer type");
	static inline UartRxBufferFreeRtos<SIZE> rxBuffer;

	static bool
	InterruptCallback(bool first)
	{
		BaseType_t xHigherPriorityTaskWoken{false};
		if constexpr (Parent::TxBufferSize)
			xHigherPriorityTaskWoken = Parent::InterruptCallback(false);

		if (Hal::isReceiveRegisterNotEmpty())
		{
			uint8_t data;
			Hal::read(data);
			xQueueSendFromISR(rxBuffer.queue, &data, &xHigherPriorityTaskWoken);
		}
		if (first)
		{
			Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::OverrunError);
			portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
		}
		return xHigherPriorityTaskWoken;
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
	read(uint8_t &data, TickType_t timeout=portMAX_DELAY)
	{ return bool(xQueueReceive(rxBuffer.queue, &data, timeout)); }

	static std::size_t
	read(uint8_t *buffer, std::size_t length, TickType_t timeout=0)
	{
		std::size_t count{0};
		for (; count < length; ++count)
			if (not xQueueReceive(rxBuffer.queue, buffer++, timeout))
				break;
		return count;
	}

	static std::size_t
	receiveBufferSize() { return uxQueueMessagesWaiting(rxBuffer.queue); }

	static std::size_t
	discardReceiveBuffer()
	{
		std::size_t count = 0;
		uint8_t data;
		while(receiveBufferSize())
		{
			++count;
			xQueueReceive(rxBuffer.queue, &data, 0);
		}
		return count;
	}
};
/// @endcond

}
