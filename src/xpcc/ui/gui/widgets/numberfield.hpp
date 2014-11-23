// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_NUMBERFIELD_HPP
#define XPCC_GUI_NUMBERFIELD_HPP

#include "widget.hpp"

namespace xpcc {

namespace gui {

template<typename T>
class NumberField : public Widget {

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


}
}

#include "numberfield_impl.hpp"

#endif  // XPCC_GUI_NUMBERFIELD_HPP
