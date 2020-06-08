/*
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2015, 2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_UART_HPP
#define MODM_INTERFACE_UART_HPP

#include <modm/architecture/interface/peripheral.hpp>

namespace modm
{

/**
 * Interface of an Uart.
 *
 * Asynchronous and synchronous, buffered access to the Uart.
 *
 * @author	Niklas Hauser
 * @ingroup	modm_architecture_uart
 */
class Uart : public ::modm::PeripheralDriver
{
#ifdef __DOXYGEN__
public:
	/// Size of the receive buffer.
	static constexpr size_t RxBufferSize = 16;

	/// Size of the transmit buffer.
	static constexpr size_t TxBufferSize = 16;

public:
	/**
	 * Connect GPIOs to the peripheral and configure them.
	 *
	 * This configures the Tx and Rx signals as output and input and connects them.
	 *
	 * @tparam	Signals
	 *		One Tx and one Rx signal are required and can be passed out-of-order.
	 */
	template< class... Signals >
	static void
	connect();

	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	SystemClock
	 * 		the currently active system clock
	 * @tparam	baudrate
	 *		desired baud rate in Hz
	 * @tparam	tolerance
	 * 		the allowed absolute tolerance for the resulting baudrate
	 */
	template< class SystemClock, baudrate_t baudrate, percent_t tolerance=10_pct >
	static void
	initialize();

	/// Write a single byte and wait for completion.
	static void
	writeBlocking(uint8_t data);

	/// Write a block of bytes and wait for completion.
	static void
	writeBlocking(const uint8_t *data, std::size_t length);

	/// Flush the write buffer, waits in a while loop until `isWriteFinished()` returns `true`
	static void
	flushWriteBuffer();


	/**
	 * Pushes a single byte into the buffer.
	 *
	 * @return	`true` if data has been buffered, `false` if buffer is full
	 */
	static bool
	write(uint8_t data);

	/**
	 * Pushes a block of bytes into the buffer.
	 *
	 * @param	data
	 *		Pointer to a buffer big enough to store `length` bytes
	 * @param	length
	 * 		Number of bytes to be written
	 *
	 * @return	the number of bytes pushed into the buffer, maximal `length`
	 */
	static std::size_t
	write(const uint8_t *data, std::size_t length);

	/// @return	`true` if the buffer is empty and the last byte has been sent
	static bool
	isWriteFinished();

	/**
	 * Read a single byte.
	 *
	 * @param[out]	data
	 *		Byte read, if any
	 *
	 * @return	`true` if a byte was received, `false` otherwise
	 */
	static bool
	read(uint8_t &data);

	/**
	 * Read a block of bytes.
	 *
	 * @param[out]	data
	 *		Pointer to a buffer big enough to store `length` bytes
	 * @param		length
	 *		Number of bytes to be read
	 *
	 * @return	Number of bytes which could be read, maximal `length`
	 */
	static std::size_t
	read(uint8_t *data, std::size_t length);

	/// @return	the size of the receive FIFO queue.
	static std::size_t
	receiveBufferSize();

	/**
	 * Empty the receive FIFO queue and hardware buffer.
	 *
	 * @return	the size of the deleted FIFO queue.
	 */
	static std::size_t
	discardReceiveBuffer();

	/// @return	the size of the transmit FIFO queue.
	static std::size_t
	transmitBufferSize();

	/**
	 * Empty the transmit FIFO queue and hardware buffer.
	 *
	 * @return	the size of the deleted FIFO queue.
	 */
	static std::size_t
	discardTransmitBuffer();

	/// @return `true` if an error occured during any write or read
	static bool
	hasError();

	/// Reset the sticky error indication
	static void
	clearError();
#endif
};

}	// namespace modm

#endif // MODM_INTERFACE_UART_HPP
