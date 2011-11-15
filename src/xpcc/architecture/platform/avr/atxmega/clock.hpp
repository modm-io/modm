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

#ifndef XPCC_ATXMEGA__CLOCK_HPP
#define	XPCC_ATXMEGA__CLOCK_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/architecture/platform/avr.hpp>

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief	Enable an external clock on XTAL1
		 * 
		 * \see		enableExternalOscillator() for values of frequency
		 * \see		selectSystemClockSource()
		 * \ingroup	atxmega
		 */
		void
		enableExternalClock(
				OSC_FRQRANGE_t frequency = OSC_FRQRANGE_2TO9_gc);
		
		/**
		 * 
		 * Values for \c frequency:
		 * - \c OSC_FRQRANGE_04TO2_gc for 0.4..2 MHz
		 * - \c OSC_FRQRANGE_2TO9_gc for 2..9 MHz
		 * - \c OSC_FRQRANGE_9TO12_gc for 9..12 MHz
		 * - \c OSC_FRQRANGE_12TO16_gc 12..16 MHz
		 * 
		 * Values for \c startupTime:
		 * - \c OSC_XOSCSEL_XTAL_256CLK_gc
		 * - \c OSC_XOSCSEL_XTAL_1KCLK_gc
		 * - \c OSC_XOSCSEL_XTAL_16KCLK_gc
		 * 
		 * \see		selectSystemClockSource()
		 * \ingroup	atxmega
		 */
		void
		enableExternalOscillator(
				OSC_FRQRANGE_t frequency = OSC_FRQRANGE_12TO16_gc,
				OSC_XOSCSEL_t startupTime = OSC_XOSCSEL_XTAL_256CLK_gc);
		
		/**
		 * \brief	Enable PLL
		 * 
		 * The Phase Locked Loop (PLL) is able to generate a high frequency
		 * system clock. The source frequency is multiplied by a user selectable
		 * multiplication factor from 1 to 31.
		 * 
		 * The output frequency from the PLL should not exceed 200 MHz.
		 * The PLL must have a minimum output frequency of 10 MHz.
		 * 
		 * Possible values for \c source:
		 * - \c OSC_PLLSRC_RC2M_gc
		 * - \c OSC_PLLSRC_RC32M_gc
		 * - \c OSC_PLLSRC_XOSC_gc
		 * 
		 * An external clock must be minimum 0.4 MHz to be used as source clock.
		 * 
		 * If you use a switchable clock source make sure that the input source
		 * is enabled and stable before activating the PLL.
		 * When using a crystal oscillator or an external clock that is always
		 * active it'll be fine.
		 * 
		 * \param source	Source clock
		 * \param factor	Multiplication factor
		 * 
		 * \see		selectSystemClockSource()
		 * \ingroup	atxmega
		 */
		void
		enablePll(OSC_PLLSRC_t source, uint8_t factor);
		
		/**
		 * \brief	Select a new main clock source
		 * 
		 * Possible values:
		 * - internal 2 MHz  (\c CLK_SCLKSEL_RC2M_gc)
		 * - internal 32 MHz (\c CLK_SCLKSEL_RC32M_gc)
		 * - internal 32 kHz (\c CLK_SCLKSEL_RC32K_gc)
		 * - external clock or oscillator (\c CLK_SCLKSEL_XOSC_gc)
		 * - PLL (\c CLK_SCLKSEL_PLL_gc)
		 * 
		 * Example:
		 * \code
		 * // select external clock with 8MHz as clock source and set PLL
		 * // source to XOSC & factor to x4
		 * xpcc::atxmega::enableExternalClock(OSC_FRQRANGE_2TO9_gc);
		 * xpcc::atxmega::enablePll(OSC_PLLSRC_XOSC_gc, 4);
		 *
		 * // set up prescalers (=1) and select PLL as clock source (4 x 8MHz)
		 * xpcc::atxmega::setSystemClockPrescaler();
		 * xpcc::atxmega::selectSystemClockSource(CLK_SCLKSEL_PLL_gc);
		 * \endcode
		 *
		 * \ingroup	atxmega
		 */
		void
		selectSystemClockSource(CLK_SCLKSEL_t source);
		
		/**
		 * TODO documentation
		 * 
		 * \ingroup	atxmega
		 */
		void
		setSystemClockPrescaler(
				CLK_PSADIV_t prescalerA = CLK_PSADIV_1_gc,
				CLK_PSBCDIV_t prescalerBC = CLK_PSBCDIV_1_1_gc);
	}
}

#endif	// XPCC_ATXMEGA__CLOCK_HPP
