/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_UART_DEVICE_HPP
#define MODM_INTERFACE_UART_DEVICE_HPP

#include "uart.hpp"
#include <modm/processing/timer.hpp>

namespace modm
{

/**
 * Base class of an UART Device.
 *
 * This class provides generic transaction-like semantics
 *
 * @author	Rasmus Kleist Hørlyck Sørensen
 * @ingroup modm_architecture_uart_device
 */
template < class Uart, uint8_t NestingLevels = 10 >
class UartDevice : protected modm::NestedResumable< NestingLevels + 1 >
{
public:
	UartDevice() :
		txTimeout(std::chrono::microseconds(1000)),
		rxTimeout(std::chrono::microseconds(10000))
	{
	}

	bool
	hasReceived()
	{
		return Uart::receiveBufferSize() > 0;
	}

	void
	setTxTimeout(ShortPreciseDuration timeout)
	{
		txTimeout = timeout;
	}

	void
	setRxTimeout(ShortPreciseDuration timeout)
	{
		rxTimeout = timeout;
	}

protected:
	modm::ResumableResult<bool>
	write(uint8_t data)
	{
		RF_BEGIN(0);

		timeout.restart(txTimeout);
		RF_WAIT_UNTIL(Uart::write(data) or timeout.isExpired() or Uart::hasError());
		if (timeout.isExpired() or Uart::hasError())
		{
			Uart::discardTransmitBuffer();
			Uart::discardReceiveBuffer();
			Uart::clearError();
			RF_RETURN(false);
		}
		RF_END_RETURN(true);
	}

	modm::ResumableResult<bool>
	write(const uint8_t *data, std::size_t length)
	{
		RF_BEGIN(0);

		writeIndex = 0;
		timeout.restart(txTimeout);
		while (writeIndex < length)
		{
			if (size_t writeSize = Uart::write(&data[writeIndex], length - writeIndex))
			{
				writeIndex += writeSize;
				timeout.restart(txTimeout);
			}

			if (timeout.isExpired() or Uart::hasError())
			{
				Uart::discardReceiveBuffer();
				Uart::clearError();
				RF_RETURN(false);
			}
			RF_YIELD();
		}

		RF_END_RETURN(true);
	}

	modm::ResumableResult<bool>
	read(uint8_t &data)
	{
		RF_BEGIN(1);

		timeout.restart(rxTimeout);
		RF_WAIT_UNTIL(Uart::read(data) or timeout.isExpired() or Uart::hasError());
		if (timeout.isExpired() or Uart::hasError())
		{
			Uart::discardReceiveBuffer();
			Uart::clearError();
			RF_RETURN(false);
		}
		RF_END_RETURN(true);
	}

	modm::ResumableResult<bool>
	read(uint8_t *buffer, std::size_t length)
	{
		RF_BEGIN(1);

		readIndex = 0;
		timeout.restart(rxTimeout);
		while (readIndex < length)
		{
			if (size_t readSize = Uart::read(&buffer[readIndex], length - readIndex))
			{
				readIndex += readSize;
				timeout.restart(rxTimeout);
			}

			if (timeout.isExpired() or Uart::hasError())
			{
				Uart::discardReceiveBuffer();
				Uart::clearError();
				RF_RETURN(false);
			}
			RF_YIELD();
		}

		RF_END_RETURN(true);
	}

private:
	std::size_t readIndex;
	std::size_t writeIndex;

	ShortPreciseDuration txTimeout;
	ShortPreciseDuration rxTimeout;
	ShortPreciseTimeout timeout;
};

}	// namespace modm

#endif // MODM_INTERFACE_UART_DEVICE_HPP
