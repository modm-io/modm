/*
 * Copyright (c) 2009-2011, 2013, 2015, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2011-2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COMMON_CLOCK_HPP
#define MODM_COMMON_CLOCK_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>

namespace modm
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
	kHz32       =   32 * kHz1,
	kHz40       =   40 * kHz1,
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
	MHz40       =   40 * MHz1,
	MHz42       =   42 * MHz1,
	MHz48       =   48 * MHz1,
	MHz50       =   50 * MHz1,
	MHz80       =   80 * MHz1,
	MHz64       =   64 * MHz1,
	MHz72       =   72 * MHz1,
	MHz84       =   84 * MHz1,
	MHz100      =  100 * MHz1,
	MHz120      =  120 * MHz1,
	MHz168      =  168 * MHz1,
	MHz180      =  180 * MHz1,
	MHz192      =  192 * MHz1,
	MHz200      =  200 * MHz1,
	MHz216      =  216 * MHz1,
	MHz432      =  432 * MHz1,
};

static constexpr int
MHz(int f) { return (f == HzDontCare)? HzDontCare : (f / MHz1); }

modm_unused
static constexpr uint32_t
u32MHz(int f) { return static_cast<uint32_t>(MHz(f)); }

static constexpr int
kHz(int f) { return (f == HzDontCare)? HzDontCare : (f / kHz1); }

modm_unused
static constexpr uint32_t
u32kHz(int f) { return static_cast<uint32_t>(kHz(f)); }

modm_unused
static constexpr int
Hz(int f)  { return (f == HzDontCare)? HzDontCare : (f / Hz1); }

/**
 * Errors that might prevent using a certain clock configuration
 */
enum class
StartupError : uint8_t
{
	None = 0,
	InternalClock,
	ExternalClock,
	ExternalCrystal,
	LowSpeedInternalClock,
	LowSpeedExternalClock,
	LowSpeedExternalCrystal,
	Pll,
	SystemClock,
	ClockOutput,
	RealTimeClock,
	WatchdogClock
};

extern uint32_t modm_fastdata fcpu;
extern uint32_t modm_fastdata fcpu_kHz;
extern uint16_t modm_fastdata fcpu_MHz;
extern uint16_t modm_fastdata ns_per_loop;

}	// namespace clock

}	// namespace modm

#endif	// MODM_COMMON_CLOCK_HPP
