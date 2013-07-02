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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__TLC594X_HPP
#define XPCC__TLC594X_HPP

#include <stdint.h>
#include <xpcc/architecture/peripheral/gpio.hpp>
#include <xpcc/architecture/driver/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	TLC594* multi-channel, daisy-chainable, constant-current sink, 12bit PWM LED driver.
	 *
	 * This class does not use the DCPRG pin, as writing to the EEPROM requires
	 * 22V to be applied to Vprog, requiring additional external circuits.
	 * Use of any EEPROM functions is therefore not implemented here.
	 * Make sure that pin is connected to Vcc, otherwise Dot Correction does not work.
	 * 
	 * This class also does not control the BLANK pin, as the setup of the timer
	 * to drive that pin is platform and configuration specific.
	 * Switching off all LEDs using one command is not implemented here.
	 *
	 * This driver can be used for the 16-channel TLC5940 and 24-channel TLC5947
	 * and probably similar TLC59s as well, simply by adjusting the number of
	 * CHANNELS.
	 * Therefore this class can also be used with daisy-chained TLC59s, e.g.
	 * to control two TLC5940s set CHANNELS to 2*16.
	 *
	 * ####  WARNING  ####
	 *
	 * Each channel in the TLC594x chip drives a transistor using a feedback loop
	 * to make it meet a particular current requirement.
	 * If a channel is disconnected, the feedback loop will fully drive the transistor.
	 * If most of the channels are disconnected (quite common in a testing
	 * environment if not in production), this will end up pulling quite a bit 
	 * of power from the chip's 3.3 or 5v supply.
	 * This can heat up the chip and cause power supply issues.
	 *
	 *
	 * \tparam CHANNELS	Number of channels must be multiples of 4, adjust for daisy-chained chips
	 * \tparam	Spi		Spi interface
	 * \tparam	Xlat	Level triggered latch pin
	 * \tparam	Vprog	Vprog pin, use xpcc::gpio::Unused if not connected
	 * \tparam	Xerr	Error pin, use xpcc::gpio::Unused if not connected
	 * 
	 * \author	Niklas Hauser
	 * \ingroup	pwm
	 */
	template<
	uint16_t CHANNELS,
	typename Spi,
	typename Xlat,
	typename Vprog=xpcc::gpio::Unused,
	typename Xerr=xpcc::gpio::Unused>
	class TLC594X
	{
	public:
		/**
		 * \param channels	initialize channels buffer with value, disable with -1
		 * \param dots		initialize dot correction buffer with value, disable with -1
		 * \param writeCH	write channels value to chip
		 * \param writeDC	write dots value to chip
		 */
		static void
		initialize(uint16_t channels=0, uint8_t dots=63, bool writeCH=true, bool writeDC=true);
		
		/// set the 12bit value of a channel
		/// call transfer() to update the chip
		static void
		setChannel(uint16_t channel, uint16_t value);
		
		/// \param value	the 12bit value of all channels
		/// \param update	write data to chip
		static void
		setAllChannels(uint16_t value, bool update=false);
		
		/// get the stored 12bit value of a channel
		/// this does reflect the actual value in the chip
		static uint16_t
		getChannel(uint16_t channel);
		
		/// set the 6bit dot correction value of a channel
		/// call transfer() to update the chip
		static void
		setDotCorrection(uint16_t channel, uint8_t value);
		
		/// \param value the 6bit dot correction value of all channels
		/// \param update write data to chip
		static void
		setAllDotCorrection(uint8_t value, bool update=false);
		
		/// get the stored 6bit dot correction value of a channel
		/// this does reflect the actual value in the chip
		static uint8_t
		getDotCorrection(uint16_t channel);
		
		/// transfer channel data to driver chip
		static bool
		writeChannels(bool flush=true);
		
		/// transfer dot correction data to driver chip
		static bool
		writeDotCorrection();
		
		/// writes data from the input shift register to either GS  or DC register.
		inline static void
		latch();
		
		/// \return true if LOD or TEF is detected
		ALWAYS_INLINE
		static bool
		isError()
		{
			return !Xerr::read();
		}
		
		ALWAYS_INLINE
		static uint8_t*
		getGrayscaleData()
		{
			return gs;
		}
		ALWAYS_INLINE
		static uint8_t*
		getDotCorrectionData()
		{
			return dc;
		}
		ALWAYS_INLINE
		static uint8_t*
		getStatusData()
		{
			return status;
		}
		
	private:
		static uint8_t status[CHANNELS*3/2];
		static uint8_t gs[CHANNELS*3/2];
		static uint8_t dc[CHANNELS*3/4];

	};
}

#include "tlc594x_impl.hpp"

#endif // XPCC__TLC594X_HPP
