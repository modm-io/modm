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
 * $hello please remove me$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LPC__VIC_HPP
#define XPCC_LPC__VIC_HPP

namespace xpcc
{
	namespace lpc
	{
		enum InterruptSources
		{
			WDT = 0,
			ARM_CORE0 = 2,	///< Embedded ICE, DbgCommRx
			ARM_CORE1 = 3,	///< Embedded ICE, DbgCommTX
			TIMER0 = 4,
			TIMER1 = 5,
			UART0 = 6,
			UART1 = 7,
			PWM = 8,		///< PWM0 & PWM1
			I2C0 = 9,
			SPI_SPP0 = 10,
			SSP1 = 11,
			PLL = 12,
			RTC = 13,
			EINT0 = 14,
			EINT1 = 15,
			EINT2 = 16,
			EINT3 = 17,
			ADC0 = 18,
			I2C1 = 19,
			BOD = 20,
			ETHERNET = 21,
			USB = 22,
			CAN = 23,
			SD_MMC = 24,
			GP_DMA = 25,
			TIMER2 = 26,
			TIMER3 = 27,
			UART2 = 28,
			UART3 = 29,
			I2C2 = 30,
			I2S = 31,
		};
		
		/**
		 * @brief	Vectored Interrupt Controller
		 * 
		 * @date	June 2011
		 * @author	Fabian Greif
		 */
		class Vic
		{
			static void
			initialize();
		};
	}
}

#endif // XPCC_LPC__VIC_HPP
