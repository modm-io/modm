// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_CHECKBOX_HPP
#define XPCC_GUI_CHECKBOX_HPP

#include "widget.hpp"

namespace xpcc
{

namespace gui
{

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

}	// namespace xpcc

#endif  // XPCC_GUI_CHECKBOX_HPP
