/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Läufer
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

using namespace modm::gui;

class Overview : public modm::gui::View
{
public:

	Overview(modm::gui::GuiViewStack* stack, uint8_t identifier);

	~Overview() {}

	bool
	hasChanged();

public:
	modm::gui::TabPanel tabpanel;
	modm::gui::ButtonWidget buttonLeft;
	modm::gui::ButtonWidget buttonMiddle;
	modm::gui::ButtonWidget buttonRight;

};

#endif /* OVERVIEW_HPP_ */
