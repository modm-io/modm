// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_COMMON_CLOCK_HPP
#define XPCC_COMMON_CLOCK_HPP

#include <stdint.h>

namespace xpcc
{

namespace clock
{

/**
 * Frequencies enum to make code more readable for common
 * frequencies (in kHz).
 */
enum Frequency
{
	HzDontCare  =         -1,
	Hz1         = static_cast<uint32_t>(1),
	kHz1        = 1000 *  Hz1,
	kHz10       =   10 * kHz1,
	kHz20       =   20 * kHz1,
	kHz50       =   50 * kHz1,
	kHz100      =  100 * kHz1,
	kHz125      =  125 * kHz1,
	kHz250      =  250 * kHz1,
	kHz500      =  500 * kHz1,
	MHz1        = 1000 * kHz1,
	MHz2        =    2 * MHz1,
	MHz4        =    4 * MHz1,
	MHz8        =    8 * MHz1,
	MHz10       =   10 * MHz1,
	MHz12       =   12 * MHz1,
	MHz16       =   16 * MHz1,
	MHz17       =   17 * MHz1,
	MHz18       =   18 * MHz1,
	MHz20       =   20 * MHz1,
	MHz24       =   24 * MHz1,
	MHz25       =   25 * MHz1,
	MHz26       =   26 * MHz1,
	MHz30       =   30 * MHz1,
	MHz32       =   32 * MHz1,
	MHz36       =   36 * MHz1,
	MHz42       =   42 * MHz1,
	MHz48       =   48 * MHz1,
	MHz50       =   50 * MHz1,
	MHz64       =   64 * MHz1,
	MHz72       =   72 * MHz1,
	MHz100      =  100 * MHz1,
	MHz120      =  120 * MHz1,
	MHz168      =  168 * MHz1,
	MHz192      =  192 * MHz1,
	MHz432      =  432 * MHz1,
};

static constexpr int
MHz(int f) { return (f == HzDontCare)? HzDontCare : (f / MHz1); }
static constexpr uint32_t
u32MHz(int f) { return static_cast<uint32_t>(MHz(f)); }
static constexpr int
kHz(int f) { return (f == HzDontCare)? HzDontCare : (f / kHz1); }
static constexpr uint32_t
u32kHz(int f) { return static_cast<uint32_t>(kHz(f)); }
static constexpr int
Hz(int f)  { return (f == HzDontCare)? HzDontCare : (f / Hz1); }

/**
 * Errors that might prevent using a certain clock configuration
 */
enum class StartupError
{
	None,				// all went well
	InternalClock,		// was not able to start the internal clock
	ExternalClock,		// was not able to start the external clock
	ExternalCrystal,	// was not able to start the external oscillator
	Pll,				// was not able to start the PLL
	SystemClock,		// was not able to switch system clock input
};

}	// namespace clock

}	// namespace xpcc

#endif	// XPCC_COMMON_CLOCK_HPP
