/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_GUI_VIEW_HPP
#define XPCC_GUI_VIEW_HPP

#include "../display/graphic_display.hpp"

#include "types.hpp"
#include "widgets/widget.hpp"
#include "colorpalette.hpp"

#include "../menu/abstract_view.hpp"

namespace xpcc
{

namespace gui
{

// forward declaration
class GuiViewStack;

/**
 * The View class is the base class for all screens handled by the ViewStack class
 *
 * @ingroup	gui
 * @author	Thorsten Lajewski
 */
class View : public xpcc::AbstractView
{
	friend class GuiViewStack;

public:
	/**
	 * @param stack pointer to the stack, the screen should be displayed on.
	 * @param identifier can be used to determine which screen is the currently
	 *        displayed on the graphicDisplay
	 */
	View(xpcc::gui::GuiViewStack* stack, uint8_t identifier, xpcc::gui::Dimension dimension);

	virtual
	~View() = 0;

	/// May be called as often as possible.
	/// Handles input events located in the parent GuiViewStack.
	virtual void
	update();

	virtual void
	preUpdate()
	{
	}

	virtual void
	postUpdate()
	{
	}

	/// Draw determine the output on the Graphic Display
	virtual void
	draw();

	/// Add widget to view
	bool
	pack(Widget *w, const xpcc::glcd::Point &coord);

	/// Remove the view from the screen. The viewStack handles the deletion.
	void
	remove();

	/// Set color palette for every contained widget
	void
	setColorPalette(ColorPalette& cp);

	ColorPalette&
	getColorPalette()
	{
		return this->colorpalette;
	}

	void
	markDirty();

	void
	markDrawn();

	inline xpcc::gui::GuiViewStack*
	getViewStack()
	{
		return stack;
	}

protected:
	xpcc::gui::GuiViewStack* stack;
	Dimension dimension;
	WidgetContainer widgets;

	xpcc::gui::ColorPalette colorpalette;
};

}	// namespace gui

}	// namespace xpcc

#endif  // XPCC_GUI_VIEW_HPP
