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

#ifndef XPCC__ATMEGA_ADC_HPP
#define XPCC__ATMEGA_ADC_HPP

namespace xpcc
{
	/**
	 * \brief	Analog/Digital-converter
	 * 
	 * ADC clock frequency should be between 50 and 200 kHz.
	 * 
	 * \code
	 * #include <xpcc/architecture/atmega/adc.hpp>
	 * 
	 * int
	 * main()
	 * {
	 *     xpcc::Adc::initialize(
	 *             xpcc::Adc::REFERENCE_INTERNAL_2V56,
	 *             xpcc::Adc::PRESCALER_8);
	 *     
	 *     ...
	 *     // read channel 2
	 *     uint16_t value = xpcc::Adc::convert(2);
	 *     ...
	 * }
	 * \endcode
	 * 
	 * \code
	 * // start a new conversion on channel 3
	 * xpcc::Adc::startConversion(3);
	 * 
	 * while (!xpcc::Adc::isFinished())
	 *     ;
	 * 
	 * // read the converted value
	 * uint16_t value = xpcc::Adc::getValue();
	 * \endcode
	 * 
	 * \author	Fabian Greif
	 * \ingroup	atmega
	 */
	class Adc
	{
	public:
		enum Reference
		{
			REFERENCE_AREF = 0,
			REFERENCE_AVCC = 0x40,
			REFERENCE_INTERNAL_1V1 = 0x80,
			REFERENCE_INTERNAL_2V56 = 0xc0
		};
		
		enum Prescaler
		{
			PRESCALER_2 = 0x01,
			PRESCALER_4 = 0x02,
			PRESCALER_8 = 0x03,
			PRESCALER_16 = 0x04,
			PRESCALER_32 = 0x05,
			PRESCALER_64 = 0x06,
			PRESCALER_128 = 0x07
		};
		
	public:
		/**
		 * \brief	Initialize the A/D converter
		 */
		static void
		initialize(Reference referenceVoltage,
				Prescaler prescaler);
		
		/**
		 * \brief	Read the value an analog channel
		 * 
		 * A normal conversion takes 13 ADC clock cycles. With a clock frequency
		 * of for example 200 kHz a conversion therefore needs 65 microseconds.
		 * This time increases with a lower frequency.
		 */
		static uint16_t
		readChannel(uint8_t channel);
		
		/// Start a new conversion
		static void
		startConversion(uint8_t channel);
		
		/// Check if the conversion is finished
		static bool
		isFinished();
		
		/// Read the converte analog value
		static uint16_t
		getValue();
	};
}

#endif	// XPCC__ATMEGA_ADC_HPP
