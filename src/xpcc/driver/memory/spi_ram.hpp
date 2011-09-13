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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SPI_RAM_HPP
#define XPCC__SPI_RAM_HPP

namespace xpcc
{
	/**
	 * \brief	256kBit (64kB) Serial SRAM
	 * 
	 * Works with 23K256 (and other).
	 * 
	 * - max. 20 MHz SPI
	 * - 2.7-3.6V supply voltage
	 * 
	 * \see		<a href="http://ww1.microchip.com/downloads/en/DeviceDoc/22100D.pdf">Datasheet</a>
	 * 
	 * \ingroup	memory
	 * \author	Fabian Greif
	 */
	template <typename Spi, typename Cs, typename Hold>
	class SpiRam
	{
	public:
		/**
		 * \brief	Initialize
		 * 
		 * Sets used pins as output and switches to continuous operation mode.
		 * 
		 * \return	\c true if the device is accessible and the change to
		 * 			continuous mode was successful, \c false otherwise.
		 */
		static bool
		initialize();
		
		/**
		 * \brief	Start a read transaction
		 * 
		 * Make sure to call finish() when all data is written!
		 * 
		 * \param	address		15-bit address
		 */
		static void
		startWrite(uint16_t address);
		
		/// Write one byte
		static void
		writeByte(uint8_t data);
		
		/// Write a block of data
		static void
		write(const uint8_t *data, std::size_t size);
		
		/**
		 * \brief	Start a read transaction
		 * 
		 * Make sure to call finish() when all data is read!
		 * 
		 * \param	address		15-bit address
		 */
		static void
		startRead(uint16_t address);
		
		/// Read one byte
		static uint8_t
		readByte();
		
		/// Read a block of data
		static void
		read(uint8_t *data, std::size_t size);
		
		/**
		 * \brief	Hold communication status
		 * 
		 * The device must stay selected during the hold period (CS low). All
		 * pins will go into high impedance state.
		 * 
		 * \see		resume()
		 */
		static inline void
		suspend();
		
		/**
		 * \brief	Resume communication
		 * \see		suspend()
		 */
		static inline void
		resume();
		
		/**
		 * \brief	Complete transaction
		 * 
		 * Must be called after a transaction is completed to deselect the 
		 * device and release the bus for other operations.
		 */
		static void
		finish();
		
		
		/**
		 * \brief	Convenience function
		 * 
		 * Same as:
		 * \code
		 * startWrite(address);
		 * write(data, size);
		 * finish();
		 * \endcode
		 */
		void
		writeData(uint16_t address, const uint8_t *data, std::size_t size);
		
		/**
		 * \brief	Convenience function
		 * 
		 * Same as:
		 * \code
		 * startRead(address);
		 * read(data, size);
		 * finish();
		 * \endcode
		 */
		void
		readData(uint16_t address, uint8_t *data, std::size_t size);
		
	protected:
		/// Possible SPI instruction codes
		enum Instruction
		{
			READ = 0x03,
			WRITE = 0x02,
			READ_STATUS_REGISTER = 0x05,
			WRITE_STATUS_REGISTER = 0x01
		};
		
		/// Content of the status register
		enum StatusRegister
		{
			BYTE_MODE = 0x00,
			PAGE_MODE = 0x80,
			SEQUENTIAL_MODE = 0x40,
			
			HOLD = (1 << 1),	///< 0 = enable hold pin, 1 = disable hold pin
		};
		
		static Spi spi;
		static Cs cs;
		static Hold hold;
	};
}

#include "spi_ram_impl.hpp"

#endif // XPCC__SPI_RAM_HPP
