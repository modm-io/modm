// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
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

typedef GpioOutput0_7 Led;
typedef GpioInput3_2 Button;


typedef SystemClock<Pll<ExternalCrystal<MHz12>, MHz48> > defaultSystemClock;

#endif	// XPCC_LPC_XPRESSO_11C24_HPP
