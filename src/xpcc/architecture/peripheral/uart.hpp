// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIPHERAL_UART_HPP
#define XPCC_PERIPHERAL_UART_HPP

#include "../peripheral.hpp"

/**
 * @ingroup 	peripheral
 * @defgroup	uart	Universal Asynchronous Receiver/Transmitter (UART)
 */

namespace xpcc
{

/**
 * Interface of an Uart.
 *
 * Asynchronous and synchronous, buffered access to the Uart.
 *
 * @author	Niklas Hauser
 * @ingroup	uart
 */
class Uart : public ::xpcc::Peripheral
{
public:
	/**
	 * Commonly used baudrates.
	 *
	 * Most Serial-to-USB converters only support baudrates up to 115200 Baud
	 */
	enum
	Baudrate
	{
		    B300 =     300,
		    B600 =     600,
		   B1200 =    1200,
		   B1800 =    1800,
		   B2400 =    2400,
		   B4800 =    4800,
		   B9600 =    9600,
		  B14400 =   14400,
		  B19200 =   19200,
		  B28800 =   28800,
		  B38400 =   38400,
		  B57600 =   57600,
		  B76800 =   76800,
		 B115200 =  115200,
		 B230400 =  230400,
		 B250000 =  250000,
		 B500000 =  500000,
		B1000000 = 1000000
	};

#ifdef __DOXYGEN__
public:
	/**
	 * Initializes the hardware and sets the baudrate.
	 *
	 * @tparam	baudrate
	 *		desired baud rate in Hz
	 */
	template< uint32_t baudrate >
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

	/**
	 * Empty the receive FIFO queue and hardware buffer.
	 *
	 * @return	the size of the deleted FIFO queue.
	 */
	static std::size_t
	discardReceiveBuffer();

	/**
	 * Empty the transmit FIFO queue and hardware buffer.
	 *
	 * @return	the size of the deleted FIFO queue.
	 */
	static std::size_t
	discardTransmitBuffer();
#endif
};

}	// namespace xpcc

#endif // XPCC_PERIPHERAL_UART_HPP
