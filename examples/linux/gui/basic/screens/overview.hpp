/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef OVERVIEW_HPP_
#define OVERVIEW_HPP_


#include <modm/ui/gui/view.hpp>
#include <modm/ui/gui/view_stack.hpp>

#include <modm/ui/gui.hpp>

using namespace xpcc::gui;

class Overview : public xpcc::gui::View
{
public:

	Overview(xpcc::gui::GuiViewStack* stack, uint8_t identifier);

	~Overview() {}

	bool
	hasChanged();

public:
	xpcc::gui::TabPanel tabpanel;
	xpcc::gui::ButtonWidget buttonLeft;
	xpcc::gui::ButtonWidget buttonMiddle;
	xpcc::gui::ButtonWidget buttonRight;

};

#endif /* OVERVIEW_HPP_ */
