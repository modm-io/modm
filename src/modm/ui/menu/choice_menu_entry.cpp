/*
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

#include "choice_menu_entry.hpp"


modm::ChoiceMenuEntry::ChoiceMenuEntry(const char* text, uint16_t space, bool *valuePtr, bool defaultValue):
	text(text, space), valuePtr(valuePtr), defaultValue(defaultValue)
{

}
