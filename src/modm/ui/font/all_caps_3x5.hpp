/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, 2015, Sascha Schade
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FONT_ALL_CAPS_3X5_HPP
#define	MODM_FONT_ALL_CAPS_3X5_HPP

#include <modm/architecture/interface/accessor.hpp>

namespace modm
{
	namespace font
	{
		/**
		 * \brief	All Caps 3x5
		 *
		 * - preferred width : 3
		 * - height          : 5
		 * - hspace          : 1
		 * - vspace          : 1
		 * - first char      : 32
		 * - last char       : 128
		 * - number of chars : 96
		 * - size in bytes   : 370
		 *
		 * \ingroup	modm_ui_display_font
		 */
		EXTERN_FLASH_STORAGE(uint8_t AllCaps3x5[]);
	}
}

#endif	// MODM_FONT_ALL_CAPS_3X5_HPP

