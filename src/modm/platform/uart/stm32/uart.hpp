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

#ifndef MODM_STM32_BUFFERED_UART_HPP
#define MODM_STM32_BUFFERED_UART_HPP

#include <modm/architecture/interface/uart.hpp>
#include <modm/platform/gpio/connector.hpp>
#include <modm/architecture/interface/atomic_lock.hpp>
#include <modm/processing/fiber.hpp>
#include "uart_base.hpp"

namespace modm::platform
{

/**
 * Universal asynchronous receiver transmitter (with modular buffers)
 *
 * @author      Kevin Laeufer
 * @author      Niklas Hauser
 * @author      Dima Barsky
 * @ingroup     modm_platform_uart
 */
template< class Hal, typename... Buffers>
struct BufferedUart : public UartBase, public ::modm::Uart
{
	static constexpr size_t RxBufferSize = 0;
	static constexpr size_t TxBufferSize = 0;

	template< class... Signals >
	static void
	connect(Gpio::InputType InputTypeRx = Gpio::InputType::PullUp,
			Gpio::OutputType OutputTypeTx = Gpio::OutputType::PushPull)
	{
		using Connector = GpioConnector<Hal::UartPeripheral, Signals...>;
		using Tx = typename Connector::template GetSignal< Gpio::Signal::Tx >;
		using Rx = typename Connector::template GetSignal< Gpio::Signal::Rx >;
		static_assert(((Connector::template IsValid<Tx> and Connector::template IsValid<Rx>) and sizeof...(Signals) == 2) or
					  ((Connector::template IsValid<Tx> or  Connector::template IsValid<Rx>) and sizeof...(Signals) == 1),
					  "BufferedUart::connect() requires one Tx and/or one Rx signal!");

		Tx::setOutput(true);
		Tx::setOutput(OutputTypeTx);
		Rx::setInput(InputTypeRx);
		Connector::connect();
	}

	/// @warning Remember to set word length correctly when using the parity bit!
	template< class SystemClock, baudrate_t baudrate, percent_t tolerance=pct(1) >
	static inline void
	initialize(Hal::Parity parity=Hal::Parity::Disabled, Hal::WordLength length=Hal::WordLength::Bit8)
	{
		Hal::template initialize<SystemClock, baudrate, tolerance>(parity, length);
		Hal::enableInterruptVector(true, 12);
		Hal::setTransmitterEnable(true);
		Hal::setReceiverEnable(true);
		Hal::enableOperation();
	}

	static void
	writeBlocking(uint8_t data)
	{
		modm::this_fiber::poll([&]{ return Hal::isTransmitRegisterEmpty(); });
		Hal::write(data);
	}

	static void
	writeBlocking(const uint8_t *data, std::size_t length)
	{
		while (length-- != 0) {
			writeBlocking(*data++);
		}
	}

	static void
	flushWriteBuffer()
	{
		modm::this_fiber::poll([&]{ return isWriteFinished(); });
	}

	static bool
	write(uint8_t data)
	{
		if(!Hal::isTransmitRegisterEmpty())
			return false;

		Hal::write(data);
		return true;
	}

	static std::size_t
	write(const uint8_t *data, std::size_t length)
	{
		uint32_t i = 0;
		for (; i < length; ++i)
		{
			if (!write(*data++)) {
				return i;
			}
		}
		return i;
	}

	static bool
	isWriteFinished() { return Hal::isTransmitRegisterEmpty(); }

	static std::size_t
	transmitBufferSize() { return Hal::isTransmitRegisterEmpty() ? 0 : 1; }

	static std::size_t
	discardTransmitBuffer() { return 0; }

	static bool
	read(uint8_t &data)
	{
		if(!Hal::isReceiveRegisterNotEmpty())
			return false;

		Hal::read(data);
		return true;
	}

	static std::size_t
	read(uint8_t *buffer, std::size_t) { return read(*buffer) ? 1 : 0; }

	static std::size_t
	receiveBufferSize() { return Hal::isReceiveRegisterNotEmpty() ? 1 : 0; }

	static std::size_t
	discardReceiveBuffer() { return 0; }

	static bool
	hasError()
	{
		return Hal::getInterruptFlags().any(
			Hal::InterruptFlag::ParityError |
#ifdef USART_ISR_NE
			Hal::InterruptFlag::NoiseError |
#endif
			Hal::InterruptFlag::OverrunError | Hal::InterruptFlag::FramingError);
	}

	static void
	clearError()
	{
		return Hal::acknowledgeInterruptFlags(
			Hal::InterruptFlag::ParityError |
#ifdef USART_ISR_NE
			Hal::InterruptFlag::NoiseError |
#endif
			Hal::InterruptFlag::OverrunError | Hal::InterruptFlag::FramingError);
	}
};

}

#endif // MODM_STM32_BUFFERED_UART_HPP
