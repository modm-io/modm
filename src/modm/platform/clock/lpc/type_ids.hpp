/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, 2013, Fabian Greif
 * Copyright (c) 2012, 2014-2016, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LPC_CLOCK_TYPE_IDS_HPP
#define MODM_LPC_CLOCK_TYPE_IDS_HPP

namespace modm
{
	namespace lpc
	{
		namespace TypeId
		{
			typedef struct{} InternalClock;
			typedef struct{} ExternalClock;
			typedef struct{} ExternalCrystal;
			typedef struct{} Pll;
			typedef struct{} SystemClock;
			typedef struct{} ClockOutput;
		}
	}
}

#endif /* MODM_LPC_CLOCK_TYPE_IDS_HPP */
