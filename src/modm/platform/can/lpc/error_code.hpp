/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, 2014, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013-2014, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LPC11_ERROR_HPP
#define MODM_LPC11_ERROR_HPP

namespace modm
{
	namespace lpc
	{
		enum ErrorCode
		{
			CAN_ERROR_PASSIVE_MODE = 0x0010,
			CAN_BUS_OFF_MODE = 0x0011,
			CAN_RX_OVERFLOW = 0x0012,
			CAN_FIFO1_OVERFLOW = 0x0013,
			CAN_TX_OVERFLOW = 0x0014,
		};
	}
}

#endif // MODM_LPC11_ERROR_HPP
