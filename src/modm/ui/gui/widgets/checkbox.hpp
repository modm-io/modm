/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GUI_CHECKBOX_HPP
#define MODM_GUI_CHECKBOX_HPP

#include "widget.hpp"

namespace modm
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class CheckboxWidget : public Widget
{
public:
	CheckboxWidget(bool initial, Dimension d) :
		Widget(d, true),
		state(initial)
	{
		this->cb_activate = &click_cb;
	}

	void
	render(View* view);

	bool
	getState() { return this->state; }

	void
	setState(bool s) { this->state = s; }

private:
	static void
	click_cb(const InputEvent& ev, Widget* w, void* data);

private:
	bool state;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_CHECKBOX_HPP
