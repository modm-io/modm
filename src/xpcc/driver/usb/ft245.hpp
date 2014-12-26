// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__FT245_HPP
#define XPCC__FT245_HPP

#include "../../../drivers.hpp"
#include <xpcc/architecture/peripheral/gpio.hpp>


namespace xpcc
{
	/**
	 * \brief	FT245 USB FIFO
	 *
	 * The FT245R USB FIFO from Future Technology Devices International is a
	 * single chip USB to parallel FIFO bidirectional data transfer interface.
	 *
	 * \section ft245_example Example
	 *
	 * \code
	 * #include <xpcc/architecture.hpp>
	 * #include <xpcc/communication/ft245.hpp>
	 *
	 * GPIO__PORT(FtPort, A);
	 * GPIO__OUTPUT(FtRd, B, 0);
	 * GPIO__OUTPUT(FtWr, B, 1);
	 * GPIO__INPUT(FtTxe, B, 2);
	 * GPIO__INPUT(FtRxf, B, 3);
	 *
	 * MAIN_FUNCTION
	 * {
	 *     xpcc::Ft245<FtPort, FtRd, FtWr, FtRxf, FtTxe> ft245;
	 *
	 *     uint8_t out = 0;
	 *     while(1)
	 *     {
	 *          ft245.write(out++);
	 *
	 *     	    char loop;
	 *     	    if (ft245.read(loop)) {
	 *              ft245.write(0xff ^ loop);
	 *     	    }
	 *     }
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
		Ft245()
		{
			this->initialize();
		}

		/**
		 * \brief	Initialize the port pins.
		 *
		 * This function is called by the constructor to initialize all port pins
		 * correctly and sets the default values.
		 */
		static void
		initialize();

		/**
		 * \brief	Write a single byte to the FIFO
		 *
		 * \param	data	Single byte to write
		 */
		static void
		write(uint8_t data);

		/**
		 * \brief	Write a block of bytes to the FIFO
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
		 * \brief	Read a single byte from the FIFO
		 *
		 * \param	c		Byte read, if any
		 *
		 * \return	\c true if a byte was received, \c false otherwise
		 */
		static bool
		read(uint8_t &c);

		/**
		 * \brief	Read a block of bytes from the FIFO
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
