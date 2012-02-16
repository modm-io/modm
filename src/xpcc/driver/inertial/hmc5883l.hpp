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

#ifndef XPCC__HMC5883L_HPP
#define XPCC__HMC5883L_HPP

#include "hmc58.hpp"

namespace xpcc
{
	/**
	 * \brief	Driver for the HMC5883L digital compass.
	 *
	 * This class extends the HMC58* family driver only by the addition of
	 * device specific Register bit masks.
	 *
	 * \see Hmc58
	 *
	 * For further information on this device consult the
	 * <a href="http://www.insed.de/HMC5883L.pdf">
	 * datasheet</a>.
	 *
	 * \ingroup inertial
	 * \author	Niklas Hauser
	 *
	 * \tparam I2C Asynchronous Two Wire interface
	 */
	template < typename I2C >
	class Hmc5883l : public Hmc58< I2C >
	{
	public:
		/// device specific data output rate options of REGISTER_CONFIG_A
		enum DataOutputRate {
			DATA_OUTPUT_RATE_0_75_gc = 0x00,
			DATA_OUTPUT_RATE_1_5_gc = 0x04,
			DATA_OUTPUT_RATE_3_gc = 0x08,
			DATA_OUTPUT_RATE_7_5_gc = 0x0c,
			DATA_OUTPUT_RATE_15_gc = 0x10,
			DATA_OUTPUT_RATE_30_gc = 0x14,
			DATA_OUTPUT_RATE_75_gc = 0x18
		};
		
		/// device specific data gain options of REGISTER_CONFIG_B
		enum Gain {
			GAIN_0_88_gc,
			GAIN_1_3_gc,
			GAIN_1_9_gc,
			GAIN_2_5_gc,
			GAIN_4_0_gc,
			GAIN_4_7_gc,
			GAIN_5_6_gc,
			GAIN_8_1_gc
		};
		
		/// \brief	Constructor, sets address to default of 0x1e
		Hmc5883l() :
			Hmc58<I2C>(0x1e)
		{
		}
	};
}

#endif // XPCC__HMC5883L_HPP
