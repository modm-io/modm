// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "stringrocker.hpp"

void
xpcc::gui::StringRocker::next()
{
	if(string_id >= end)
	{
		string_id = start;
	}
	else {
		string_id++;
	}

	this->string_field.setValue(this->getValue());

	if(cb_activate != NULL)
	{
		cb_activate(InputEvent(), this, cbData);
	}

}

void
xpcc::gui::StringRocker::previous()
{
	if(string_id <= start)
	{
		string_id = end;
	}
	else {
		string_id--;
	}

	this->string_field.setValue(this->getValue());

	if(cb_activate != NULL)
	{
		cb_activate(InputEvent(), this, cbData);
	}

}

void
xpcc::gui::StringRocker::activate(const InputEvent& ev, void* /*data*/)
{
	this->handleInputEvent(&ev);
}

void
xpcc::gui::StringRocker::deactivate(const InputEvent& ev, void* /*data*/)
{
	this->handleInputEvent(&ev);
}

void
xpcc::gui::StringRocker::next_cb(const InputEvent& /*ev*/, Widget* w, void* /*data*/)
{
	StringRocker *rocker = static_cast<StringRocker*>(w->parent);

	rocker->next();
}

void
xpcc::gui::StringRocker::previous_cb(const InputEvent& /*ev*/, Widget* w, void* /*data*/)
{
	StringRocker *rocker = static_cast<StringRocker*>(w->parent);

	rocker->previous();
}
