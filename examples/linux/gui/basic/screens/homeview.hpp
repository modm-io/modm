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

#ifndef HOMEVIEW_HPP_
#define HOMEVIEW_HPP_

#include <modm/ui/gui/view.hpp>
#include <modm/ui/gui/view_stack.hpp>

#include <modm/ui/gui.hpp>

class HomeView : public xpcc::gui::View
{
public:

	HomeView(xpcc::gui::GuiViewStack* stack, uint8_t identifier);

	~HomeView() {}

	bool
	hasChanged();
private:
	xpcc::gui::ButtonWidget toggleLedButton;
	xpcc::gui::ButtonWidget doNothingButton;
	xpcc::gui::NumberRocker<int16_t> rocker1;
	xpcc::gui::CheckboxWidget checkbox1;

};

#endif /* HOMEVIEW_HPP_ */
