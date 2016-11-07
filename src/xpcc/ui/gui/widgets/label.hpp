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

#ifndef XPCC_GUI_LABEL_HPP
#define XPCC_GUI_LABEL_HPP

#include "widget.hpp"

namespace xpcc
{

namespace gui
{

/**
 * @ingroup	gui
 * @author	Daniel Krebs
 */
class Label : public Widget
{
public:
	Label(const char* lbl, xpcc::glcd::Color color) :
		Widget(Dimension(0,0), false),
		label(lbl),
		color(color)
	{
		this->updateDimension();
	}

	void
	render(View* view);

	void
	setColor(xpcc::glcd::Color color)
	{
		this->color = color;
		this->markDirty();
	}

	void
	setLabel(const char* lbl)
	{
		this->label = lbl;
		this->updateDimension();
		this->markDirty();
	}

	void
	setFont(const xpcc::accessor::Flash<uint8_t> *font)
	{
		this->font = *font;
		this->updateDimension();
	}

	void
	setFont(const uint8_t *newFont)
	{
		this->font = xpcc::accessor::asFlash(newFont);
		this->updateDimension();
	}

private:
	void
	updateDimension()
	{
		// Update label dimension
		if(this->font.isValid())
		{
			this->dimension.width = xpcc::GraphicDisplay::getStringWidth(this->label, &(this->font));
			this->dimension.height = xpcc::GraphicDisplay::getFontHeight(&(this->font));
		}
	}

private:
	const char* label;
	xpcc::glcd::Color color;
};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_LABEL_HPP
