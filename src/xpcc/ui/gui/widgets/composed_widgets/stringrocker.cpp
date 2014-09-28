#include "stringrocker.hpp"

void xpcc::gui::StringRocker::next()
{

	if(string_id >= end)
	{
		string_id = start;
	} else {
		string_id++;
	}

	this->string_field.setValue(this->getValue());

	if(cb_activate != NULL)
	{
		cb_activate(InputEvent(), this, cbData);
	}

}

void xpcc::gui::StringRocker::previous()
{
	if(string_id <= start)
	{
		string_id = end;
	} else {
		string_id--;
	}

	this->string_field.setValue(this->getValue());

	if(cb_activate != NULL)
	{
		cb_activate(InputEvent(), this, cbData);
	}

}

void xpcc::gui::StringRocker::activate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(&ev);
}

void xpcc::gui::StringRocker::deactivate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(&ev);
}

void xpcc::gui::StringRocker::next_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	StringRocker *rocker = static_cast<StringRocker*>(w->parent);

	rocker->next();
}

void xpcc::gui::StringRocker::previous_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	StringRocker *rocker = static_cast<StringRocker*>(w->parent);

	rocker->previous();
}
