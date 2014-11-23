// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "tab_panel.hpp"

void xpcc::gui::TabPanel::packPanel(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->panelDimension == w->getDimension()))
		return;

	this->pack(w, xpcc::glcd::Point(0,0));
}

void xpcc::gui::TabPanel::packTabLeft(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->buttonDimension == w->getDimension()))
		return;

	this->pack(w, xpcc::glcd::Point(0,panelDimension.height));
}

void xpcc::gui::TabPanel::packTabMiddle(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->buttonDimension == w->getDimension()))
		return;

	this->pack(w, xpcc::glcd::Point(buttonDimension.width, panelDimension.height));
}

void xpcc::gui::TabPanel::packTabRight(Widget* w)
{
	if(w == NULL)
		return;

	if(!(this->buttonDimension == w->getDimension()))
		return;

	this->pack(w, xpcc::glcd::Point(2*buttonDimension.width, panelDimension.height));
}
