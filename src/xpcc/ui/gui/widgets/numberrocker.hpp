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
#define MODM_GUI_NUMBERROCKER_HPP

#include "widget.hpp"
#include "button.hpp"
#include "numberfield.hpp"

namespace modm
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
template<typename T = int16_t>
class NumberRocker : public WidgetGroup
{
public:
	NumberRocker(T default_value, T step, Dimension d) :
		WidgetGroup(d),
		value(default_value),
		step(step),
		button_increase(true, Dimension(d.height, d.height)),
		button_decrease(false, Dimension(d.height, d.height)),
		num_field(default_value, Dimension(d.width - 2*d.height, d.height))
	{
		button_decrease.cb_activate = &decrease_cb;
		button_increase.cb_activate = &increase_cb;

		this->pack(&button_decrease, modm::glcd::Point(0,0));
		this->pack(&button_increase, modm::glcd::Point(d.width - d.height,0));
		this->pack(&num_field, modm::glcd::Point(d.height, 0));
	}

	void
	increase();

	void
	decrease();

	void
	activate(const InputEvent& ev, void* data);

	void
	deactivate(const InputEvent& ev, void* data);

	T
	getValue()
	{
		return this->value;
	}

private:
	static void
	increase_cb(const InputEvent& ev, Widget* w, void* data);

	static void
	decrease_cb(const InputEvent& ev, Widget* w, void* data);

private:
	T value;
	T step;
	ArrowButton button_increase, button_decrease;
	NumberField<T> num_field;
};

typedef NumberRocker<int16_t> IntegerRocker;
typedef NumberRocker<float> FloatRocker;


}	// namespace gui

}	// namespace modm

#include "numberrocker_impl.hpp"

#endif  // MODM_GUI_NUMBERROCKER_HPP
