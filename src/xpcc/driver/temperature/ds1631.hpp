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

#ifndef XPCC__DS1631_HPP
#define XPCC__DS1631_HPP

#include <xpcc/driver/connectivity/i2c/sync_master.hpp>

namespace xpcc
{
	/**
	 * \see		Ds1631
	 * \ingroup	temperature
	 */
	namespace ds1631
	{
		enum Resolution
		{
			RESOLUTION_9BIT = 0x00, 	///< 9 Bit
			RESOLUTION_10BIT = 0x04,	///< 10 Bit
			RESOLUTION_11BIT = 0x08,	///< 11 Bit
			RESOLUTION_12BIT = 0x0c		///< 12 Bit
		};
	}
		
	/**
	 * \brief	DS1631 Temperature Sensor
	 * 
	 * \see		ds1631
	 * 
	 * \ingroup	temperature
	 * \author	Fabian Greif
	 */
	template <typename I2C>
	class Ds1631
	{
	public:
		/**
		 * \brief	Constructor
		 * 
		 * \param	address		Default address is 0x90.
		 */
		Ds1631(uint8_t address);
		
		/**
		 * \brief	Configure DS1631
		 * 
		 * \code
		 * Resolution | Conversion Time
		 * -----------+----------------
		 *     9 Bit  |  93.75ms
		 *    10 Bit  | 187.50ms
		 *    11 Bit  | 375.00ms
		 *    12 Bit  | 750.00ms 
		 * \endcode
		 * 
		 * \param resolution		Conversion result resolution
		 * \param continuousMode	Enable the continuous mode
		 */
		void
		configure(ds1631::Resolution resolution, bool continuousMode);
		
		/**
		 * \brief	Perform a software reset
		 */
		void
		reset();
		
		/**
		 * \brief	Initiates temperature conversions
		 * 
		 * If the part is in one-shot mode, only one conversion is
		 * performed. In continuous mode, continuous temperature
		 * conversions are performed until a Stop Convert command is
		 * issued.
		 */
		void
		startConversion();
		
		/**
		 * \brief	Stops temperature conversions when the device is in
		 * 			continuous conversion mode.
		 */
		void
		stopConversion();
		
		/**
		 * \brief	Check if the conversion is done
		 */
		bool
		isConversionDone();
		
		/**
		 * \return	Temperature in degree
		 * 
		 * Output format is Q7.8
		 */
		int16_t
		readTemperature();

		/**
		 * \brief	Check if the device is accessable
		 *
		 * \return	\c true the device responds to its address,
		 * 			\c false otherwise, i.a. if bus was not free, statrcondition failed or device did not responded.
		 */
		bool
		isAvailable() const;
	private:
		const uint8_t deviceAddress;
		typedef xpcc::i2c::SyncMaster<I2C> MySyncI2C;
	};
}

#include "ds1631_impl.hpp"

#endif // XPCC__DS1631_HPP
