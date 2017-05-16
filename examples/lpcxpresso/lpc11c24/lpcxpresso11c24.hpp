/*
 * Copyright (c) 2014, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// LPC11C24 LPCXpresso Board
// http://www.embeddedartists.com/products/lpcxpresso/lpc11C24_xpr.php
//

#ifndef MODM_LPC_XPRESSO_11C24_HPP
#define MODM_LPC_XPRESSO_11C24_HPP

using namespace modm::lpc;
using namespace modm::cortex;

typedef GpioOutput0_7 Led;
typedef GpioInput3_2 Button;


typedef SystemClock<Pll<ExternalCrystal<MHz12>, MHz48> > defaultSystemClock;

#endif	// MODM_LPC_XPRESSO_11C24_HPP
