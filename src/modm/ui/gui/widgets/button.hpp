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

#ifndef MODM_GUI_BUTTON_HPP
#define MODM_GUI_BUTTON_HPP

#include <modm/ui/display/font.hpp>
#include "widget.hpp"

namespace modm
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class ButtonWidget : public Widget
{
public:
	ButtonWidget(const char* lbl, Dimension d) :
		Widget(d, true),
		label(lbl)
	{
	}

	void
	render(View* view);

	void
	setLabel(char* lbl)
	{
		this->label = lbl;
	}

private:
	const char *label;
};

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class ArrowButton : public Widget
{
public:
	ArrowButton(bool orientation, Dimension d) :
		Widget(d, true),
		orientation(orientation)
	{
	}

	void
	render(View* view);

private:
	// Orientation of the arrow. false: left | true: right
	bool orientation;
};

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class FilledAreaButton : public Widget
{
public:
	FilledAreaButton(modm::glcd::Color color, Dimension d) :
		Widget(d, true),
		color(color)
	{
	}

	void
	setBackgroundColor(modm::glcd::Color color)
	{
		this->color = color;
	}

	void
	render(View* view);

private:
	modm::glcd::Color color;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_BUTTON_HPP
