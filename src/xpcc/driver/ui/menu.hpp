// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/**
 * \defgroup display_menu Display Menu
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
 * Center: Edit selected entry
 *
 *\warning Some classes currently only work with the font
 *         xpcc::font::FixedWidth5x8
 */

#ifndef XPCC__MENU_HPP
#define XPCC__MENU_HPP

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

#endif	// XPCC__MENU_HPP
