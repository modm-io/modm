// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_NUMBERROCKER_HPP
#   error	"Don't include this file directly, use 'numberrocker.hpp' instead!"
#endif

template<typename T>
void xpcc::gui::NumberRocker<T>::increase()
{
	this->value += this->step;
	this->num_field.setValue(this->value);
	this->num_field.markDirty();

	if(cb_activate != NULL)
	{
		cb_activate(InputEvent(), this, cbData);
	}

}

template<typename T>
void xpcc::gui::NumberRocker<T>::decrease()
{
	this->value -= this->step;
	this->num_field.setValue(this->value);
	this->num_field.markDirty();

	if(cb_activate != NULL)
	{
		cb_activate(InputEvent(), this, cbData);
	}
}

template<typename T>
void xpcc::gui::NumberRocker<T>::activate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(&ev);
}

template<typename T>
void xpcc::gui::NumberRocker<T>::deactivate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(&ev);
}

template<typename T>
void xpcc::gui::NumberRocker<T>::increase_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	NumberRocker *rocker = static_cast<NumberRocker*>(w->parent);

	rocker->increase();
}

template<typename T>
void xpcc::gui::NumberRocker<T>::decrease_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	NumberRocker *rocker = static_cast<NumberRocker*>(w->parent);

	rocker->decrease();
}
