/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CHOICE_MENU_ENTRY
#define MODM_CHOICE_MENU_ENTRY

#include <stdint.h>
#include "scrollable_text.hpp"

namespace modm
{
	/// @ingroup modm_ui_menu
	class ChoiceMenuEntry
	{
	public:
		ChoiceMenuEntry(const char* text, uint16_t space, bool *valuePtr, bool defaultValue = true);
		ScrollableText text;
		bool *valuePtr;
		bool defaultValue;
	};
}
#endif // MODM_CHOICE_MENU_ENTRY
