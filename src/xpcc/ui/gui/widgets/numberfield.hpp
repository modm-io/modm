/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_NUMBERFIELD_HPP
#define XPCC_GUI_NUMBERFIELD_HPP

#include "widget.hpp"

namespace xpcc
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
template<typename T>
class NumberField : public Widget
{
public:
	NumberField(T default_value, Dimension d) :
		Widget(d, false),
		value(default_value)
	{
	}

	void
	render(View* view);

	void
	setValue(T value)
	{
		if(this->value == value)
			return;
		this->value = value;
		this->markDirty();
	}

	T
	getValue()
	{
		return this->value;
	}

private:
	T value;
};

typedef NumberField<int16_t> IntegerField;


class FloatField : public NumberField<float>
{
public:
	FloatField(float value, Dimension d);

	void
	render(View* view);
};

}	// namespace gui

}	// namespace xpcc

#include "numberfield_impl.hpp"

#endif  // XPCC_GUI_NUMBERFIELD_HPP
