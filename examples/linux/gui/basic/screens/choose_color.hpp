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

#ifndef CHOOSE_COLOR_HPP_
#define CHOOSE_COLOR_HPP_


#include <modm/ui/gui/view.hpp>
#include <modm/ui/gui/view_stack.hpp>

#include <modm/ui/gui.hpp>

using namespace modm::gui;

class ChooseColorView : public modm::gui::View
{
public:

	ChooseColorView(modm::gui::GuiViewStack* stack, uint8_t identifier);

	~ChooseColorView() {}

	bool
	hasChanged();

	static void
	activate_yellow(const InputEvent&, Widget*, void*);

	static void
	deactivate_yellow(const InputEvent&, Widget*, void*);

	static void
	activate_red(const InputEvent&, Widget*, void*);

	static void
	deactivate_red(const InputEvent&, Widget*, void*);

public:
//	modm::gui::ButtonWidget button1;
	modm::gui::Label lbl_yellow;
	modm::gui::Label lbl_red;
	modm::gui::FilledAreaButton yellow;
	modm::gui::FilledAreaButton red;


};

#endif /* CHOOSE_COLOR_HPP_ */
