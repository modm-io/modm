/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GUI_NUMBERROCKER_HPP
#   error	"Don't include this file directly, use 'numberrocker.hpp' instead!"
#endif

template<typename T>
void
modm::gui::NumberRocker<T>::increase()
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
void
modm::gui::NumberRocker<T>::decrease()
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
void
modm::gui::NumberRocker<T>::activate(const InputEvent& ev, void* /*data*/)
{
	this->handleInputEvent(&ev);
}

template<typename T>
void
modm::gui::NumberRocker<T>::deactivate(const InputEvent& ev, void* /*data*/)
{
	this->handleInputEvent(&ev);
}

template<typename T>
void
modm::gui::NumberRocker<T>::increase_cb(const InputEvent& /*ev*/, Widget* w, void* /*data*/)
{
	NumberRocker *rocker = static_cast<NumberRocker*>(w->parent);

	rocker->increase();
}

template<typename T>
void
modm::gui::NumberRocker<T>::decrease_cb(const InputEvent& /*ev*/, Widget* w, void* /*data*/)
{
	NumberRocker *rocker = static_cast<NumberRocker*>(w->parent);

	rocker->decrease();
}
