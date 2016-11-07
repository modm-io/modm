/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, 2015, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FONT__NUMBERS_14X32_HPP
#define	XPCC_FONT__NUMBERS_14X32_HPP

#include <xpcc/architecture/driver/accessor.hpp>

namespace xpcc
{
	namespace font
	{
		/**
		 * \brief	Numbers 14x32
		 * 
		 * - fixed width     : 14
		 * - height          : 32
		 * - hspace          : 3
		 * - vspace          : 4
		 * - first char      : 48
		 * - last char       : 58
		 * - number of chars : 10
		 * - size in bytes   : 578
		 * 
		 * \ingroup	font
		 */
		EXTERN_FLASH_STORAGE(uint8_t Numbers14x32[]);
	}
}

#endif	// XPCC_FONT__NUMBERS_14X32_HPP

