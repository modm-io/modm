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


#include <xpcc/ui/gui/view.hpp>
#include <xpcc/ui/gui/view_stack.hpp>

#include <xpcc/ui/gui.hpp>

using namespace xpcc::gui;

class ChooseColorView : public xpcc::gui::View
{
public:

	ChooseColorView(xpcc::gui::GuiViewStack* stack, uint8_t identifier);

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
//	xpcc::gui::ButtonWidget button1;
	xpcc::gui::Label lbl_yellow;
	xpcc::gui::Label lbl_red;
	xpcc::gui::FilledAreaButton yellow;
	xpcc::gui::FilledAreaButton red;


};

#endif /* CHOOSE_COLOR_HPP_ */
