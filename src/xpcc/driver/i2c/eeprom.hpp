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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__EEPROM_HPP
#define XPCC_I2C__EEPROM_HPP

#include "i2c.hpp"

namespace xpcc
{
	namespace i2c
	{
		/**
		 * \brief	I2C Eeprom
		 * 
		 * Compatible with the 24C256 family and other I2C eeprom with an
		 * 16-bit address pointer.
		 * 
		 * \ingroup	i2c
		 * \author	Fabian Greif
		 */
		template <typename I2C>
		class Eeprom
		{
		public:
			Eeprom(uint8_t address);
			
			/**
			 * \brief	Check if the device is accessable
			 * 
			 * \return	\c true if the device responds to its address,
			 * 			\c false otherwise.
			 */
			bool
			isAvailable() const;
			
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
			write(uint16_t address, uint8_t data) const;
			
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
			write(uint16_t address, const uint8_t *data, uint8_t bytes) const;
			
			/// Read byte
			bool
			read(uint16_t address, uint8_t &data) const;
			
			/// Read block
			bool
			read(uint16_t address, uint8_t *data, uint8_t bytes) const;
			
		protected:
			uint8_t deviceAddress;
			
			static I2C i2c;
		};
	}
}

#include "eeprom_impl.hpp"

#endif // XPCC_I2C__EEPROM_HPP
