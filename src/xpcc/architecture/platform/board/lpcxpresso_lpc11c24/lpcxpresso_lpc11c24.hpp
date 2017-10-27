// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// LPC11C24 LPCXpresso Board
// http://www.embeddedartists.com/products/lpcxpresso/lpc11C24_xpr.php
//

#ifndef XPCC_LPC_XPRESSO_11C24_HPP
#define XPCC_LPC_XPRESSO_11C24_HPP

using namespace xpcc::lpc;
using namespace xpcc::cortex;

namespace Board
{

using clockSource = Pll<ExternalCrystal<MHz12>, MHz48> ;
using systemClock = SystemClock<clockSource>;

using LedRed = GpioOutput0_7;

using Leds = xpcc::SoftwareGpioPort< LedRed >;

inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	LedRed::setOutput(xpcc::Gpio::Low);
}

} // Board namespace

#endif	// XPCC_LPC_XPRESSO_11C24_HPP
