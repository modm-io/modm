/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
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

#ifndef XPCC__MENU_ENTRY_CALLBACK
#define XPCC__MENU_ENTRY_CALLBACK

#include "abstract_menu.hpp"

namespace xpcc
{
	class MenuEntryCallback
	{
	public:
			typedef void (xpcc::AbstractMenu::*Function)();

			template <typename M>
			MenuEntryCallback(M *menu, void (M::*function)()) :
				menu(reinterpret_cast<xpcc::AbstractMenu *>(menu)),
				function(reinterpret_cast<Function>(function))
			{
			}

			inline void
			call() const
			{
				(menu->*function)();
			}

		protected:
			xpcc::AbstractMenu * const menu;
			Function const function;
		};
}
#endif // XPCC__MENU_ENTRY_CALLBACK
