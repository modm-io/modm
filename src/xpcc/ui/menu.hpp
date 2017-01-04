/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2009, 2013, Thorsten Lajewski
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * \defgroup display_menu Display Menu
 * \ingroup	ui
 *
 * \brief Simple to use classes for implementing menu structures
 *        on graphic displays.
 *
 * Some convenience classes for implementing menus. The menus are
 * using a stack to allow stepping forward and backwards through the
 * screens.
 * The menus are based on the usage of five buttons. Inside most of the
 * screens these buttons have the following functions:
 * Left: Go back one screen.
 * Right: Go to next screen.
 * Up: Go to previous selected entry on screen
 * Down: Go to next entry on screen
 * OK: Edit selected entry
 *
 *\warning Some classes currently only work with the font
 *         xpcc::font::FixedWidth5x8
 */

#ifndef XPCC_MENU_HPP
#define XPCC_MENU_HPP

#include "menu/view_stack.hpp"
#include "menu/abstract_view.hpp"
#include "menu/abstract_menu.hpp"
#include "menu/communicating_view.hpp"
#include "menu/communicating_view_stack.hpp"
#include "menu/menu_buttons.hpp"
#include "menu/standard_menu.hpp"
#include "menu/menu_entry_callback.hpp"
#include "menu/scrollable_text.hpp"
#include "menu/choice_menu.hpp"
#include "menu/choice_menu_entry.hpp"

#endif	// XPCC_MENU_HPP
