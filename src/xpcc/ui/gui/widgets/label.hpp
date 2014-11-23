// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_LABEL_HPP
#define XPCC_GUI_LABEL_HPP

#include "widget.hpp"

namespace xpcc
{

namespace gui
{

class Label : public Widget {

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
		/* Update label dimension */
		if(this->font.isValid()) {
			this->dimension.width = xpcc::GraphicDisplay::getStringWidth(this->label, &(this->font));
			this->dimension.height = xpcc::GraphicDisplay::getFontHeight(&(this->font));
		}
	}

private:
	const char* label;
	xpcc::glcd::Color color;

};

//
//class InteractiveLabel : public Label {
//
//public:
//	InteractiveLabel(const char* lbl) :
//		Label(lbl)
//	{
//		this->is_interactive = true;
//	}
//};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_LABEL_HPP
