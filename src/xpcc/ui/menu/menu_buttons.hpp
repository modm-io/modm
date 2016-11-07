/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MENU_BUTTONS_HPP
#define XPCC__MENU_BUTTONS_HPP

namespace xpcc
{
	namespace MenuButtons
	{
	   /**
		 *
		 * @brief The Button enum defines the five buttons, which are used in the views.
		 *
		 * \author Thorsten Lajewski
		 *
		 * \ingroup display_menu
		 */
		enum Button
		{
			LEFT,
			RIGHT,
			UP,
			DOWN,
			OK
		};
	}
}

#endif /* XPCC__MENU_BUTTONS_HPP */
