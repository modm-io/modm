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

#ifndef XPCC__ATXMEGA_TIMER_HPP
#define XPCC__ATXMEGA_TIMER_HPP

#include <avr/io.h>

#ifndef __DOXYGEN__

#define TIMER_COUNTER(name, port, type) \
	struct name { \
		ALWAYS_INLINE static TC ## type ## _t &get() { return TC ## port ## type; } \
		ALWAYS_INLINE static void setClockSource(TC_CLKSEL_t selection=TC_CLKSEL_DIV1024_gc) { \
			TC ## port ## type ## _CTRLA = selection; } \
		ALWAYS_INLINE static void setWaveformGenerationMode(TC_WGMODE_t mode=TC_WGMODE_NORMAL_gc) { \
			TC ## port ## type ## _CTRLB = (TC ## port ## type ## _CTRLB & TC ## type ## _WGMODE_gm) | mode; } \
		ALWAYS_INLINE static void setCCEnable(uint8_t selection) { \
			TC ## port ## type ## _CTRLB = (TC ## port ## type ## _CTRLB & 0x0f) | selection; } \
		ALWAYS_INLINE static void setCompareOutputValue(uint8_t selection) { \
			TC ## port ## type ## _CTRLC = selection; } \
		ALWAYS_INLINE static void setEventSource(TC_EVSEL_t selection=TC_EVSEL_OFF_gc) { \
			TC ## port ## type ## _CTRLD = (TC ## port ## type ## _CTRLD & ~TC ## type ## _EVSEL_gm) | selection; } \
		ALWAYS_INLINE static void setDelayEvent(bool enable=true) { \
			TC ## port ## type ## _CTRLD = (TC ## port ## type ## _CTRLD & ~TC ## type ## _EVDLY_bm) | (enable?TC ## type ## _EVDLY_bm:0); } \
		ALWAYS_INLINE static void setEventAction(TC_EVACT_t action=TC_EVACT_OFF_gc) { \
			TC ## port ## type ## _CTRLD = (TC ## port ## type ## _CTRLD & ~TC ## type ## _EVSEL_gm) | action; } \
		ALWAYS_INLINE static void setByteMode(bool enable=true) { \
			TC ## port ## type ## _CTRLE = (TC ## port ## type ## _CTRLE & ~TC ## type ## _BYTEM_bm) | (enable?TC ## type ## _BYTEM_bm:0); } \
		ALWAYS_INLINE static void setTimerInterruptLevel(uint8_t level) { \
			TC ## port ## type ## _INTCTRLA = level; } \
		ALWAYS_INLINE static void setCCInterruptLevel(uint8_t level) { \
			TC ## port ## type ## _INTCTRLB = level; } \
		ALWAYS_INLINE static void setTimerCommand(uint8_t command, bool clear=false) { \
			if (clear) { TC ## port ## type ## _CTRLFCLR = command; } \
			else { TC ## port ## type ## _CTRLFSET = command; } } \
		ALWAYS_INLINE static bool isTimerCountingUp() { \
			return (!(TC ## port ## type ## _CTRLFSET & TC ## type ## _DIR_bm)); } \
		\
		struct AWEX { \
			ALWAYS_INLINE static AWEX_t &get() { return AWEX ## port; } \
			ALWAYS_INLINE static void setMode(uint8_t mode) { \
				AWEX ## port ## _CTRL = (AWEX ## port ## _CTRL & ~(AWEX_PGM_bm|AWEX_CWCM_bm)) | mode; } \
			ALWAYS_INLINE static void setDTIEnable(uint8_t selection) { \
				AWEX ## port ## _CTRL = (AWEX ## port ## _CTRL & ~(AWEX_DTICCDEN_bm|AWEX_DTICCCEN_bm|AWEX_DTICCBEN_bm|AWEX_DTICCAEN_bm)) | selection; } \
			ALWAYS_INLINE static void setFaultDetection(uint8_t mode) { \
				AWEX ## port ## _FDCTRL = mode; } \
			ALWAYS_INLINE static uint8_t getStatus() { return AWEX ## port ## _STATUS; } \
			}; \
		\
		struct HIRES { \
			ALWAYS_INLINE static HIRES_t &get() { return HIRES ## port; } \
			ALWAYS_INLINE static void setEnable(HIRES_HREN_t enable) { \
				HIRES ## port ## _CTRL = enable; } \
			}; \
		};

#else   // __DOXYGEN__

/**
 * Universal Timer/Counter
 *
 * Combines TC0, TC1, AWEX and HIRES for each Timer/Counter.
 * TC type 0 has got four Compare/Capture Units, type 1 only two.
 *
 * Be aware that both type 0 and 1 share the same AWEX and HIRES Extension!
 *
 * Since this is meant to be used in combination with templates, it is missing
 * access to all the data registers and some methods might be incomplete.
 * The Xmega Timer/Counters are very complex hardware.
 * It is recommended that you consult the Xmega Manual and write your own
 * template to configure the Timer/Counter and its Extensions.
 *
 * Creates something like this (type 0):
 * \code
 * class name
 * {
 * public:
 *     static inline TC0_t
 *     &get();
 *
 *     // Enable the SPI module.
 *     static inline void
 *     setClockSource(TC_CLKSEL_t selection=TC_CLKSEL_DIV1024_gc);
 *
 *     static inline void
 *     setWaveformGenerationMode(TC_WGMODE_t mode=TC_WGMODE_NORMAL_gc);
 *
 *     // Choose between enabling Capture or Compare mode
 *     static inline void
 *     setCCEnable(uint8_t);
 *
 *     static inline void
 *     setCompareOutputValue(uint8_t);
 *
 *     static inline void
 *     setEventSource(TC_EVSEL_t selection=TC_EVSEL_OFF_gc);
 *
 *     static inline void
 *     setDelayEvent(bool enable=true);
 *
 *     static inline void
 *     setEventAction(TC_EVACT_t action=TC_EVACT_OFF_gc);
 *
 *     // use only a 8 bit counter
 *     static inline void
 *     setByteMode(bool enable=true);
 *
 *     static inline uint8_t
 *     setTimerInterruptLevel(uint8_t level);
 *
 *     static inline void
 *     setCCInterruptLevel(uint8_t level);
 *
 *     // use this to perform TC resets
 *     static inline void
 *     setTimerCommand(uint8_t command, bool clear=false);
 *
 *     static inline bool
 *     isTimerCountingUp();
 *
 *     // Advanced Waveform Extension
 *     static inline AWEX_t
 *     AWEX::&get();
 *
 *     static inline void
 *     AWEX::setMode(uint8_t);
 *
 *     // Enable Dead Time Insertion for each CC
 *     static inline void
 *     AWEX::setDTIEnable(uint8_t);
 *
 *     static inline void
 *     AWEX::setFaultDetection(uint8_t);
 *
 *     static inline uint8_t
 *     AWEX::getStatus();
 *
 *     // High Resolution Extension, makes TC run 4x faster
 *     static inline HIRES_t
 *     HIRES::&get();
 *
 *     // Enable Extension on a CC
 *     static inline void
 *     HIRES::setEnable(HIRES_HREN_t);
 * };
 * \endcode
 *
 * \author Niklas Hauser
 * \ingroup	atxmega
 */
#define	TIMER_COUNTER(name, timer, type)


#endif  // __DOXYGEN__
#endif	// XPCC__ATXMEGA_TIMER_HPP
