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

ChooseColorView::ChooseColorView(modm::gui::GuiViewStack* stack,
		uint8_t identifier) :
		View(stack, identifier, modm::gui::Dimension(320, 240)),
//		button1("Test", modm::gui::Dimension(80, 30)),
		lbl_yellow("YELLOW", colorpalette[modm::gui::Color::BLACK]),
		lbl_red("RED", colorpalette[modm::gui::Color::BLACK]),
		yellow(colorpalette[modm::gui::Color::YELLOW], modm::gui::Dimension(159, 240)),
		red(colorpalette[modm::gui::Color::RED], modm::gui::Dimension(160, 240))
{

	this->pack(&yellow, modm::glcd::Point(0, 0));
	this->pack(&red, modm::glcd::Point(160, 0));

	this->pack(&lbl_yellow, modm::glcd::Point(20, 40));
	this->pack(&lbl_red, modm::glcd::Point(210, 40));

	lbl_yellow.setFont(modm::font::Ubuntu_36);
	lbl_red.setFont(modm::font::Ubuntu_36);

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
	yellow->setBackgroundColor(yellow->color_palette[modm::gui::Color::GREEN]);
}

void ChooseColorView::deactivate_yellow(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto yellow = static_cast<FilledAreaButton*>(w);
	yellow->setBackgroundColor(yellow->color_palette[modm::gui::Color::YELLOW]);
}

void ChooseColorView::activate_red(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto red = static_cast<FilledAreaButton*>(w);
	red->setBackgroundColor(red->color_palette[modm::gui::Color::GREEN]);
}

void ChooseColorView::deactivate_red(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto red = static_cast<FilledAreaButton*>(w);
	red->setBackgroundColor(red->color_palette[modm::gui::Color::RED]);
}
