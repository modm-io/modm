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

#ifndef MODM_GUI_STRINGFIELD_HPP
#define MODM_GUI_STRINGFIELD_HPP

#include "widget.hpp"
#include "label.hpp"

namespace modm
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class StringField : public Widget
{
public:
	StringField(const char* value, Dimension d) :
		Widget(d, false),
		value(value)
	{
	}

	void
	render(View* view);

	void
	setValue(const char* value)
	{
		if(this->value == value)
			return;
		this->value = value;
		this->markDirty();
	}

	const char*
	getValue()
	{
		return this->value;
	}

private:
	const char* value;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_STRINGFIELD_HPP
