/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Dave Webb
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__FT245_HPP
#define XPCC__FT245_HPP

#include <modm/architecture/interface/gpio.hpp>

namespace xpcc
{
	/**
	 * FT245 USB FIFO
	 *
	 * The FT245R USB FIFO from Future Technology Devices International is a
	 * single chip USB to parallel FIFO bidirectional data transfer interface.
	 *
	 * Wr and Rd must be GpioOutputs and set to output mode before.
	 * Rxf and Txe must be GpioInputs.
	 * Port is switched between input and output mode.
	 *
	 * \section ft245_example Example
	 *
	 * \code
	 * #include <modm/architecture.hpp>
	 * #include <modm/driver/usb/ft245.hpp>
	 *
	 * typedef xpcc::stm32::GpioPort<xpcc::stm32::GpioD0, 8> myPort;
	 * typedef xpcc::stm32::GpioInputC11  Rxf;
	 * typedef xpcc::stm32::GpioInputC10  Txe;
	 * typedef xpcc::stm32::GpioOutputA15 Rd;
	 * typedef xpcc::stm32::GpioOutputA9  Wr;
	 *
	 * typedef xpcc::Ft245<myPort, Rd, Wr, Rxf, Txe> myFt;
	 *
	 * int
	 * main()
	 * {
	 *   Rd::setOutput(xpcc::Gpio::High);
	 *   Wr::setOutput(xpcc::Gpio::Low);
	 *
	 *   while (1)
	 *   {
	 *     uint8_t c;
	 *     if (myFt::read(c)) {
	 *     myFt::write(c + 1);
	 *   }
	 * }
	 * \endcode
	 *
	 * \ingroup	driver_other
	 */
	template < typename PORT,
	           typename RD,
	           typename WR,
	           typename RXF,
	           typename TXE >
	class Ft245
	{
	public:
		/// Write a single byte to the FIFO
		/// \param	data	Single byte to write
		static bool
		write(uint8_t data);

		/**
		 * Write a block of bytes to the FIFO
		 *
		 * This blocks until the buffer is written.
		 *
		 * \param	*buffer	Buffer of the data that should be written
		 * \param	nbyte	Length of buffer
		 *
		 */
		static void
		write(const uint8_t *buffer, uint8_t nbyte);

		/**
		 * Read a single byte from the FIFO
		 *
		 * \param	c		Byte read, if any
		 *
		 * \return	\c true if a byte was received, \c false otherwise
		 */
		static bool
		read(uint8_t &c);

		/**
		 * Read a block of bytes from the FIFO
		 *
		 * This is blocking.
		 *
		 * \param	*buffer	Buffer for the received data.
		 * \param	nbyte	Length of buffer
		 *
		 */
		static uint8_t
		read(uint8_t *buffer, uint8_t nbyte);

	protected:
		static PORT port;
		static RD   rd;
		static WR   wr;
		static RXF  rxf;
		static TXE  txe;
	};
}

#include "ft245_impl.hpp"

#endif // XPCC__FT2425_HPP
