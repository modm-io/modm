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

#include "overview.hpp"

#include <xpcc/ui/gui/widgets.hpp>

using namespace xpcc::gui;

Overview::Overview(xpcc::gui::GuiViewStack* stack, uint8_t identifier) :
	View(stack, identifier, Dimension(320, 240)),
	tabpanel(Dimension(320, 240), 0.2),
	buttonLeft("Back", tabpanel.buttonDimension),
	buttonMiddle("change", tabpanel.buttonDimension),
	buttonRight("Repos.", tabpanel.buttonDimension)
{

	tabpanel.packTabLeft(&buttonLeft);
	tabpanel.packTabMiddle(&buttonMiddle);
	tabpanel.packTabRight(&buttonRight);


	this->pack(&tabpanel, xpcc::glcd::Point(0, 0));

}

bool Overview::hasChanged()
{
	return true;
}
