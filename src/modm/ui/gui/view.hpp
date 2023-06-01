/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, 2017-2018, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
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

#ifndef MODM_GUI_VIEW_HPP
#define MODM_GUI_VIEW_HPP

#include <modm/ui/display/color_graphic_display.hpp>

#include "types.hpp"
#include "widgets/widget.hpp"
#include "colorpalette.hpp"

#include <modm/ui/menu/abstract_view.hpp>

namespace modm
{

namespace gui
{

// forward declaration
class GuiViewStack;

/**
 * The View class is the base class for all screens handled by the ViewStack class
 *
 * @ingroup modm_ui_gui
 * @author	Thorsten Lajewski
 */
class View
{
	friend class GuiViewStack;

public:
	/**
	 * @param stack pointer to the stack, the screen should be displayed on.
	 * @param identifier can be used to determine which screen is the currently
	 *        displayed on the graphicDisplay
	 */
	View(modm::gui::GuiViewStack* stack, uint8_t identifier, modm::gui::Dimension dimension);

	virtual
	~View() = 0;

	/// May be called as often as possible.
	/// Handles input events located in the parent GuiViewStack.
	virtual void
	update();

	/**
	 * @brief hasChanged indicates the current displayed view has changed.
	 *        This function prevents unnecessary drawing of the display
	 * @return if true the display has to be redrawn.
	 */
	virtual bool
	hasChanged() = 0;

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

	/**
	 * @brief shortButtonPress handle the action for the pressed button
	 */
	virtual void
	shortButtonPress(modm::MenuButtons::Button /*button*/)
	{
		// nothing to be done
	}

	/// Add widget to view
	bool
	pack(Widget *w, const modm::glcd::Point &coord);

	/**
	 * @brief isAlive tells the ViewStack if it should remove this screen.
	 * @return
	 */
	bool
	isAlive() const
	{
		return this->alive;
	}

	/// Remove the view from the screen. The viewStack handles the deletion.
	void
	remove();

	/**
	 * @brief onRemove will be called right before the view gets deleted,
	 *        can be reimplemented to reset external data.
	 */
	virtual void
	onRemove()
	{
		// nothing to be done here
	}

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

	inline modm::gui::GuiViewStack*
	getViewStack()
	{
		return stack;
	}

	modm::ColorGraphicDisplay&
	display();

	/**
	 * @brief getIdentifier of the view.
	 */
	inline uint8_t getIdentifier(){
		return this->identifier;
	}

protected:
	modm::gui::GuiViewStack* stack;
	Dimension dimension;
	WidgetContainer widgets;

	modm::gui::ColorPalette colorpalette;

	const uint8_t identifier;
	bool alive;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_VIEW_HPP
