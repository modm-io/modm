// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#ifndef XPCC__I2C_EEPROM_HPP
#define XPCC__I2C_EEPROM_HPP

#include <xpcc/communication/i2c/delegate.hpp>

namespace xpcc
{
	/**
	 * \brief	I2C Eeprom
	 * 
	 * Compatible with the 24C256 (ST) and 24FC1025 (Microchip) family and other
	 * I2C eeprom with an 16-bit address pointer.
	 * Base address for most 24xxyyyy eeproms is 0xA0.
	 * 
	 * \ingroup	storage
	 * \author	Fabian Greif
	 * \author	Niklas Hauser
	 */
	template <typename I2cMaster>
	class I2cEeprom : public xpcc::i2c::Delegate
	{
	public:
		I2cEeprom(uint8_t address);
		
		/**
		 * \brief	Write byte
		 * 
		 * \param	address		Address
		 * \param	data		Data byte
		 * 
		 * \return	\c true	if the data could be written,
		 * 			\c false otherwise
		 */
		bool
		writeByte(uint16_t address, uint8_t data);
		
		/**
		 * \brief	Write block
		 * 
		 * \param	address		Address
		 * \param	data		Data block
		 * \param	bytes		Number of bytes to be written
		 * 
		 * \return	\c true	if the data could be written,
		 * 			\c false otherwise
		 */
		bool
		write(uint16_t address, const uint8_t *data, uint8_t bytes);
		
		/**
		 * \brief	Convenience function
		 *  
		 * Shortcut for:
		 * \code
		 * return write(address, static_cast<const uint8_t *>(&data), sizeof(T));
		 * \endcode
		 */
		template <typename T>
		inline bool
		write(uint16_t address, const T& data);
		
		/// Read byte
		bool
		readByte(uint16_t address, uint8_t &data);
		
		/// Read block
		bool
		read(uint16_t address, uint8_t *data, uint8_t bytes);
		
		/**
		 * \brief	Convenience function
		 * 
		 * Shortcut for:
		 * \code
		 * return read(address, static_cast<uint8_t *>(&data), sizeof(T));
		 * \endcode
		 */
		template <typename T>
		inline bool
		read(uint16_t address, T& data);
		
		bool
		isAvailable();
		
	private:
		uint8_t address;
		uint8_t readSize;
		uint8_t writeSize;
		uint8_t auxWriteSize;
		uint8_t *readBuffer;
		const uint8_t *writeBuffer;
		const uint8_t *auxWriteBuffer;
		bool twoBuffers;
		volatile xpcc::i2c::adapter::State state;
		bool isReading;
		
		uint8_t buffer[3];
		
	private:
		bool
		initialize(const uint8_t* writeBuffer, uint8_t writeSize, uint8_t* readBuffer, uint8_t readSize);
		
		bool
		initialize(const uint8_t* auxWriteBuffer, uint8_t auxWriteSize, const uint8_t* writeBuffer, uint8_t writeSize, uint8_t* readBuffer, uint8_t readSize);
		
		virtual bool
		attaching();
		
		virtual Starting
		starting();
		
		virtual Writing
		writing();
		
		virtual Reading
		reading();
		
		virtual void
		stopped(DetachCause cause);
	};
}

#include "i2c_eeprom_impl.hpp"

#endif // XPCC__I2C_EEPROM_HPP
