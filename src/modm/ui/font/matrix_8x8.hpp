/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2015, Sascha Schade
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FONT_MATRIX_8X8_HPP
#define	MODM_FONT_MATRIX_8X8_HPP

#include <modm/architecture/interface/accessor.hpp>

namespace modm
{
	namespace font
	{
		/**
		 * \brief	Matrix 8x8
		 *
		 * - fixed width     : 8
		 * - height          : 8
		 * - hspace          : 0
		 * - vspace          : 0
		 * - first char      : 48
		 * - last char       : 58
		 * - number of chars : 10
		 * - size in bytes   : 98
		 *
		 * \ingroup	modm_ui_display_font
		 */
		EXTERN_FLASH_STORAGE(uint8_t Matrix8x8[]);
	}
}

#endif	// MODM_FONT_MATRIX_8X8_HPP

