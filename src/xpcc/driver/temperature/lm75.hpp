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

#ifndef XPCC__LM75_HPP
#define XPCC__LM75_HPP

#include <xpcc/communication/i2c/read_adapter.hpp>

namespace xpcc
{
	/**
	 * \brief	LM75
	 * 
	 * \ingroup	temperature
	 * \author	Fabian Greif
	 */
	template <typename I2cMaster >
	class Lm75 : protected xpcc::I2cWriteReadAdapter
	{
	public:
		/**
		 * \param	data		pointer to a 2 uint8_t buffer
		 * \param	address		Default address is 0x90.
		 */
		Lm75(uint8_t *data, uint8_t address=0x90)
		:	I2cWriteReadAdapter(address), status(0), data(data)
		{
			configureWriteRead(address << 1, data, 2);
		}
		
		/**
		 * read the temperature registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		ALWAYS_INLINE void
		readTemperature()
		{
			status |= READ_TEMPERATURE_PENDING;
		}
		
		/**
		 * \c true, when new data has been read from the sensor
		 */
		ALWAYS_INLINE bool
		isNewDataAvailable()
		{
			return status & NEW_TEMPERATURE_DATA;
		}
		
		/// \return pointer to 8bit array containing temperature
		inline uint8_t*
		getData()
		{
			newData = false;
			return data;
		}
		
		void
		update()
		{
			if (status & READ_TEMPERATURE_RUNNING &&
				getAdapterState() == xpcc::I2c::AdapterState::Idle) {
				status &= ~READ_TEMPERATURE_RUNNING;
				status |= NEW_TEMPERATURE_DATA;
			}
			if (status & READ_TEMPERATURE_PENDING) {
				if (I2cMaster::start(this)) {
					status &= ~READ_TEMPERATURE_PENDING;
					status |= READ_TEMPERATURE_RUNNING;
				}
			}
		}
		
	private:
		enum Status {
			READ_TEMPERATURE_PENDING = 0x01,
			READ_TEMPERATURE_RUNNING = 0x02,
			NEW_TEMPERATURE_DATA = 0x04,
		};
		
		uint8_t status;
		uint8_t *data;
	};
}

#endif // XPCC__LM75_HPP
