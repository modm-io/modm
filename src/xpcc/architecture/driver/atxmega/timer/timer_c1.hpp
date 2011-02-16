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
 */
// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file.
 */
// ----------------------------------------------------------------------------


#ifndef XPCC__XMEGA_TIMER_C1_HPP
#define XPCC__XMEGA_TIMER_C1_HPP

#include <avr/io.h>
#include <stdint.h>

#include "timer.hpp"
#include "hires_c.hpp"
#ifdef AWAXC
#include "awex_c.hpp"
#endif // AWAXC

#if defined(TCC1) || defined(__DOXYGEN__)

namespace xpcc
{
	typedef void (*F) (void);
	/**
	 * \brief		Timer Module C1
	 *
	 * \ingroup		atxmega_timer
	 */
#if defined(AWAXC) || defined(__DOXYGEN__)
	class TimerC1 : public HighResC, public WaveformC
#else
	class TimerC1 : public HighResC
#endif // AWAXC
	{
	public:
		inline static TC1_t&
		getModuleBase()
		{
			return TCC1;
		}
		
		inline static bool
		isType0()
		{
			return false;
		}
		
		inline static void
		setClockSource(TC_CLKSEL_t selection=TC_CLKSEL_DIV1024_gc)
		{
			TCC1_CTRLA = selection;
		}

		inline static void
		setWaveformGenerationMode(TC_WGMODE_t mode=TC_WGMODE_NORMAL_gc)
		{
			TCC1_CTRLB = (TCC1_CTRLB & TC1_WGMODE_gm) | mode;
		}
		
		inline static void
		enableCompareCapture(uint8_t selection)
		{
			TCC1_CTRLB = (TCC1_CTRLB & 0xf0) | selection;
		}

		inline static void
		setCompareOutputValue(uint8_t selection)
		{
			TCC1_CTRLC = selection;
		}

		inline static void
		setEventSource(TC_EVSEL_t selection=TC_EVSEL_OFF_gc)
		{
			TCC1_CTRLD = (TCC1_CTRLD & ~TC1_EVSEL_gm) | selection;
		}
		
		inline static void
		setDelayEvent(bool enable=true)
		{
			TCC1_CTRLD = (TCC1_CTRLD & ~TC1_EVDLY_bm) | (enable?TC1_EVDLY_bm:0);
		}
		
		inline static void
		setEventAction(TC_EVACT_t action=TC_EVACT_OFF_gc)
		{
			TCC1_CTRLD = (TCC1_CTRLD & ~TC1_EVACT_gm) | action;
		}
		
		inline static void
		setByteMode(bool enable=true)
		{
			TCC1_CTRLE = (TCC1_CTRLE & ~TC1_BYTEM_bm) | (enable?TC1_BYTEM_bm:0);
		}
		
		/// The TCC1_ERR_vect needs to be handled seperately.
		inline static void
		setErrorInterrupt(TC_ERRINTLVL_t level)
		{
			TCC1_INTCTRLA = (TCC1_INTCTRLA & ~TC1_ERRINTLVL_gm) | level;
		}
		
		/// The TCC1_OVF_vect needs to be handled seperately.
		inline static void
		setOverflowInterrupt(TC_OVFINTLVL_t level)
		{
			TCC1_INTCTRLA = (TCC1_INTCTRLA & ~TC1_OVFINTLVL_gm) | level;
		}
		
		/// The TCC1_CCA_vect needs to be handled seperately.
		inline static void
		setCompareCaptureAInterrupt(TC_CCAINTLVL_t level)
		{
			TCC1_INTCTRLB = (TCC1_INTCTRLB & ~TC1_CCAINTLVL_gm) | level;
		}
		
		/// The TCC1_CCB_vect needs to be handled seperately.
		inline static void
		setCompareCaptureBInterrupt(TC_CCBINTLVL_t level)
		{
			TCC1_INTCTRLB = (TCC1_INTCTRLB & ~TC1_CCBINTLVL_gm) | level;
		}
		
		
		
		static void
		setTimerCommand(uint8_t command, bool clear=false);
		
		inline static bool
		isTimerCountingUp()
		{
			return (!(TCC1_CTRLFSET & TC1_DIR_bm));
		}
		
		/**
		 * \brief Enable OVL interrupt in x ms intervals
		 * 
		 * Creates a medium level OVL interrupt every x ms.
		 * Sets the timer prescaler to 64 and calculates the timer period for
		 * the milliseconds specified. Interrupt handling has to be done 
		 * manually. Accuracy is not guaranteed!
		 * The maximum interval is 131ms at 32MHz, and 2097ms at 2MHz.
		 * 
		 * \param interval between interrupts in ms
		 */
		static void
		setMsTimer(uint8_t interval=1);
	};
}

#endif // TCC1
#endif // XPCC__XMEGA_TIMER_C1_HPP