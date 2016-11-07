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

#include "choose_color.hpp"

ChooseColorView::ChooseColorView(xpcc::gui::GuiViewStack* stack,
		uint8_t identifier) :
		View(stack, identifier, xpcc::gui::Dimension(320, 240)),
//		button1("Test", xpcc::gui::Dimension(80, 30)),
		lbl_yellow("YELLOW", colorpalette[xpcc::gui::Color::BLACK]),
		lbl_red("RED", colorpalette[xpcc::gui::Color::BLACK]),
		yellow(colorpalette[xpcc::gui::Color::YELLOW], xpcc::gui::Dimension(159, 240)),
		red(colorpalette[xpcc::gui::Color::RED], xpcc::gui::Dimension(160, 240))
{

	this->pack(&yellow, xpcc::glcd::Point(0, 0));
	this->pack(&red, xpcc::glcd::Point(160, 0));

	this->pack(&lbl_yellow, xpcc::glcd::Point(20, 40));
	this->pack(&lbl_red, xpcc::glcd::Point(210, 40));

	lbl_yellow.setFont(xpcc::font::Ubuntu_36);
	lbl_red.setFont(xpcc::font::Ubuntu_36);

	yellow.cb_activate = &activate_yellow;
	yellow.cb_deactivate = &deactivate_yellow;

	red.cb_activate = &activate_red;
	red.cb_deactivate = &deactivate_red;

}

bool ChooseColorView::hasChanged()
{
	return true;
}

void ChooseColorView::activate_yellow(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto yellow = static_cast<FilledAreaButton*>(w);
	yellow->setBackgroundColor(yellow->color_palette[xpcc::gui::Color::GREEN]);
}

void ChooseColorView::deactivate_yellow(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto yellow = static_cast<FilledAreaButton*>(w);
	yellow->setBackgroundColor(yellow->color_palette[xpcc::gui::Color::YELLOW]);
}

void ChooseColorView::activate_red(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto red = static_cast<FilledAreaButton*>(w);
	red->setBackgroundColor(red->color_palette[xpcc::gui::Color::GREEN]);
}

void ChooseColorView::deactivate_red(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto red = static_cast<FilledAreaButton*>(w);
	red->setBackgroundColor(red->color_palette[xpcc::gui::Color::RED]);
}
