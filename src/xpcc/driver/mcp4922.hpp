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

#ifndef XPCC__MCP4922_HPP
#define XPCC__MCP4922_HPP

namespace xpcc
{
	/**
	 * \brief	Dual-channel 12-Bit Digital-to-Analog Converter (DAC)
	 * 
	 * Features:
	 * - 12-Bit Resolution
	 * - 2.7 to 5.5V Supply Voltage
	 * - 6µs maximum settling time (0V to VREF)
	 * - 20Mhz SPI interface
	 * - Synchronous latching of both channels
	 * 
	 * \code
	 * Vout = Vref * G * Din / 2^12 = Vref * G * Din / 4096
	 * \endcode
	 * with Din = 0..4095 and G = 1..2
	 * 
	 * \see		<a href="http://ww1.microchip.com/downloads/en/devicedoc/21897a.pdf">Datasheet</a>
	 * 
	 * \ingroup	driver
	 * \author	Fabian Greif
	 */
	template <typename Spi, typename Cs, typename Ldac>
	class Mcp4922
	{
	public:
		/**
		 * \brief	Initialize
		 * 
		 * Sets used pins as output etc.
		 */
		static void
		initialize();
		
		/**
		 * \brief	Set value for channel A
		 * 
		 * \param	value		Raw value in range [0..4095]
		 * \param	doubleGain	If set it will increase the gain G from 1 to 2.
		 * 
		 * \warning	Changes are not visible at the output until update() is called!
		 * \see		update()
		 */
		static void
		setChannelA(uint16_t value, bool doubleGain = false);
		
		/**
		 * \brief	Set value for channel B
		 * 
		 * \param	value		Raw value in range [0..4095]
		 * \param	doubleGain	If set it will increase the gain G from 1 to 2.
		 * 
		 * \warning	Changes are not visible at the output until update() is called!
		 * \see		update()
		 */
		static void
		setChannelB(uint16_t value, bool doubleGain = false);
		
		/**
		 * \brief	Update the outputs
		 * 
		 * Afterwards the values set by setChannelA() and setChannelB() will
		 * be active at the two outputs.
		 */
		static void
		update();
		
	protected:
		/// Bit values of the configuration register
		enum Register
		{
			SHDN = (1 << 12),		///< Shutdown
			GA = (1 << 13),			///< Gain setting
			BUF = (1 << 14),		///< Buffered AREF
			CHANNEL_B = (1 << 15)	///< Select channel !A/B	
		};
		
		/// Write a 16-bit configuration register
		static void
		writeRegister(uint16_t value);
		
		static Spi spi;
		static Cs cs;
		static Ldac ldac;
	};
}

#include "mcp4922_impl.hpp"

#endif // XPCC__MCP4922_HPP
