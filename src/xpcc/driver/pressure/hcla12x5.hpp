// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
 * $Id: hcla12x5.hpp 738 2012-02-25 17:54:01Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_HCLA12X5_HPP
#define XPCC_HCLA12X5_HPP

#include <xpcc/driver/connectivity/i2c/master.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for the HCLA12X5 differential pressure sensor.
	 * \ingroup sensors
	 * \tparam Twi I2C interface.
	 * \author	Niklas Hauser
	 */
	template < typename TwiMaster >
	class HCLA12X5 : public xpcc::i2c::Delegate
	{
	public:
		/**
		 * \brief	Constructor
		 * \param	address		Default address is 0x78.
		 * \bug The address of the sensor is by factory default set to 0x78.
		 *      This means you cannot use two HCLA sensors on the same bus!
		 *      You have to use a MUX or two seperate TWI busses.
		 */
		HCLA12X5(uint8_t address=0x78);
		
		/**
		 * read the pressure registers and buffer the results
		 * sets isNewDataAvailable() to \c true
		 */
		bool
		readPressure();
		
		/**
		 * \c true, when new data has been from the sensor and buffered,
		 * \c false, when the data has already been read, or data is being 
		 * copied into the buffer (by readAccelerationAverage()).
		 */
		bool
		isNewDataAvailable();
		
		/// \return pointer to 8bit array containing temperature
		uint8_t*
		getData();
		
	private:
		/**
		 * this delegate function gets called after calling readTemperature()
		 * \return always \c false, since we do not want to continue using the bus
		 */
		void
		twiCompletion(const uint8_t *data, std::size_t index, bool reading);
		
		bool newData;
		uint8_t data[2];
		uint8_t deviceAddress;
	};
}

#include "hcla12x5_impl.hpp"

#endif // XPCC_HCLA12X5_HPP