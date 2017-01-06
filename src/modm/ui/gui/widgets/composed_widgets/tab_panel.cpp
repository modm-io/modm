/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "tab_panel.hpp"

void
modm::gui::TabPanel::packPanel(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->panelDimension == w->getDimension()))
		return;

	this->pack(w, modm::glcd::Point(0,0));
}

void
modm::gui::TabPanel::packTabLeft(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->buttonDimension == w->getDimension()))
		return;

	this->pack(w, modm::glcd::Point(0,panelDimension.height));
}

void
modm::gui::TabPanel::packTabMiddle(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->buttonDimension == w->getDimension()))
		return;

	this->pack(w, modm::glcd::Point(buttonDimension.width, panelDimension.height));
}

void
modm::gui::TabPanel::packTabRight(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->buttonDimension == w->getDimension()))
		return;

	this->pack(w, modm::glcd::Point(2*buttonDimension.width, panelDimension.height));
}
