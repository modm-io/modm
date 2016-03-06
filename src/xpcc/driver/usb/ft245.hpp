// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__FT245_HPP
#define XPCC__FT245_HPP

#include <xpcc/architecture/interface/gpio.hpp>

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
	 * #include <xpcc/architecture.hpp>
	 * #include <xpcc/driver/usb/ft245.hpp>
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
