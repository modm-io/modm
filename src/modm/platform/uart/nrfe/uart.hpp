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

#ifndef MODM_NRF_BUFFERED_UART_HPP
#define MODM_NRF_BUFFERED_UART_HPP

#include <modm/architecture/interface/uart.hpp>
#include <modm/platform/gpio/connector.hpp>
#include <modm/processing/fiber.hpp>
#include "uart_base.hpp"

namespace modm::platform
{

template< class Hal, typename... Buffers>
struct BufferedUart : public UartBase, public ::modm::Uart
{
	static constexpr size_t RxBufferSize = 0;
	static constexpr size_t TxBufferSize = 0;

	template< class... Signals >
	static void
	connect(Gpio::InputType inputType = Gpio::InputType::PullUp,
			Gpio::OutputType outputType = Gpio::OutputType::PushPull)
	{
		using Connector = GpioConnector<Hal::UartPeripheral, Signals...>;
		using Txd = typename Connector::template GetSignal< Gpio::Signal::Txd >;
		using Rxd = typename Connector::template GetSignal< Gpio::Signal::Rxd >;
		static constexpr bool hasTxd = not std::is_same_v<Txd, GpioUnused>;
		static constexpr bool hasRxd = not std::is_same_v<Rxd, GpioUnused>;
		static_assert(((hasTxd and hasRxd) and sizeof...(Signals) == 2) or
					  ((hasTxd or  hasRxd) and sizeof...(Signals) == 1),
					  "BufferedUart::connect() requires one Txd and/or one Rxd signal!");

		if constexpr (hasTxd) {
			Txd::setOutput(true);
			Txd::setOutput(outputType);
		}
		if constexpr (hasRxd) {
			Rxd::setInput(inputType);
		}
		Connector::connect();
		Hal::connectPins(Txd::number, Rxd::number);
	}

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

	static void writeBlocking(uint8_t data)
	{
		modm::this_fiber::poll([&]{ return Hal::isTransmitRegisterEmpty(); });
		Hal::write(data);
	}

	static void writeBlocking(const uint8_t *data, std::size_t length)
	{
		while (length-- != 0) {
			writeBlocking(*data++);
		}
	}

	static void flushWriteBuffer()
	{
		modm::this_fiber::poll([&]{ return isWriteFinished(); });
	}

	static bool write(uint8_t data)
	{
		if(!Hal::isTransmitRegisterEmpty())
			return false;

		Hal::write(data);
		return true;
	}

	static std::size_t write(const uint8_t *data, std::size_t length)
	{
		uint32_t count = 0;
		for (; count < length; ++count)
		{
			if (!write(*data++)) {
				return count;
			}
		}
		return count;
	}

	static bool isWriteFinished() { return Hal::isTransmitRegisterEmpty(); }
	static std::size_t transmitBufferSize() { return Hal::isTransmitRegisterEmpty() ? 0 : 1; }
	static std::size_t discardTransmitBuffer() { return 0; }

	static bool read(uint8_t &data)
	{
		if(!Hal::isReceiveRegisterNotEmpty())
			return false;

		Hal::read(data);
		return true;
	}

	static std::size_t read(uint8_t *buffer, std::size_t) { return read(*buffer) ? 1 : 0; }
	static std::size_t receiveBufferSize() { return Hal::isReceiveRegisterNotEmpty() ? 1 : 0; }
	static std::size_t discardReceiveBuffer() { return 0; }

	static bool hasError()
	{
		return Hal::getInterruptFlags().any(Hal::InterruptFlag::OverrunError |
										 Hal::InterruptFlag::FramingError |
										 Hal::InterruptFlag::ParityError);
	}

	static void clearError()
	{
		Hal::acknowledgeInterruptFlags(Hal::InterruptFlag::OverrunError |
								   Hal::InterruptFlag::FramingError |
								   Hal::InterruptFlag::ParityError);
	}
};

} // namespace modm::platform

#endif // MODM_NRF_BUFFERED_UART_HPP
