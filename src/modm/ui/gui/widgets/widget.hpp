/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Läufer
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

#ifndef MODM_GUI_WIDGET_HPP
#define MODM_GUI_WIDGET_HPP

#include <modm/ui/display/color_graphic_display.hpp>
#include <modm/ui/gui/colorpalette.hpp>
#include <modm/ui/gui/types.hpp>

#include "../view.hpp"

#include <modm/debug/logger.hpp>

namespace modm
{

namespace gui
{

/// @cond
static int16_t uid_global = 0;
/// @endcond

class View;

/**
 * @ingroup modm_ui_gui
 * @author	Daniel Krebs
 */
class Widget
{
public:
	Widget(Dimension dimension, bool is_interactive) :
		parent(NULL),
		dimension(dimension),
		activated(false),
		cbData(NULL),
		cb_activate(NULL),
		cb_deactivate(NULL),
		position(modm::glcd::Point(-10,-10)),
		relative_position(modm::glcd::Point(-10,-10)),
		dirty(true),
		is_interactive(is_interactive),
		font(modm::accessor::asFlash(modm::font::FixedWidth5x8))
	{
		// assign unique id
		uid = uid_global++;
	}

	virtual
	~Widget()
	{
	}

	/// Draws the widget on screen. Each widget need to implement this.
	virtual void
	render(View* view) = 0;

	/**
	 * Interface for drawing widgets. Basically calls render(), but has some
	 * logic that needs to be executed before and after rendering a widget
	 */
	void
	draw(View *view)
	{
		// render widget on screen
		this->render(view);
		this->markDrawn();

		// if there are widgets on top, redraw them
		if(this->hasIntersections())
		{
			for(auto iter = this->intersecting_widgets.begin(); iter != this->intersecting_widgets.end(); ++iter)
			{
				(*iter)->draw(view);
			}
		}
	}

	/**
	 * Handles InputEvents and calls activate/deactivate if event coordinates
	 * are within widgets dimensions. Returning true means, that the event
	 * was inside these dimensions, false if not.
	 */
	virtual bool
	handleInputEvent(const InputEvent* ev);

	/**
	 * Check if given Widget w overlaps *on top*. For widgets overlapping below
	 * this function also returns false.
	 */
	bool
	checkIntersection(Widget* w);

	/**
	 * Clear old list of intersecting widgets and rebuild on the basis of the
	 * given widgets container.
	 */
	void
	updateIntersections(WidgetContainer *widgets);

	/// Whether there are other widgets overlapping this one *on top*
	bool
	hasIntersections();

	/// Interface for activating widget. Calls callback function if specified.
	virtual void
	activate(const InputEvent& ev, void* data)
	{
		this->activated = true;
		this->dirty = true;

		// call callback when set
		if(cb_activate != NULL)
			this->cb_activate(ev, this, data);
	}

	/// Interface for deactivating widget. Calls callback function if specified.
	virtual void
	deactivate(const InputEvent& ev, void* data)
	{
		this->activated = false;
		this->dirty = true;

		// call callback when set
		if(cb_deactivate != NULL)
			this->cb_deactivate(ev, this, data);
	}

	/// Get widget-specific color palette. NOT YET USED
	ColorPalette&
	getColorPalette()
	{
		return this->color_palette;
	}

	/// Set widget-specific color palette.
	virtual void
	setColorPalette(ColorPalette& cp)
	{
		this->color_palette = cp;
		this->markDirty();
	}

	void
	setColor(modm::gui::Color name, modm::color::Rgb565 color)
	{
		this->color_palette.setColor(name, color);
		this->markDirty();
	}

	/**
	 * Set position of widget relative to its parent. Also updates the absolute
	 * position by looping through parents.
	 */
	virtual void
	setPosition(const modm::glcd::Point& pos)
	{
		this->setRelativePosition(pos);

		this->updatePosition();
	}

	/// Get absolute position of widget on screen.
	inline modm::glcd::Point
	getPosition()
	{
		return this->position;
	}

	/// Get position of widget relative to its parent.
	inline modm::glcd::Point
	getRelativePosition()
	{
		return this->relative_position;
	}

	/// Only set the relative position.
	inline void
	setRelativePosition(const modm::glcd::Point& pos)
	{
		this->relative_position = pos;
	}

	/**
	 * Updates the absolute position by looping through all parents and adding
	 * up their relative positions.
	 */
	virtual void
	updatePosition()
	{
		this->position = this->getRelativePosition();

		Widget* w = this;

		while(w->parent != NULL)
		{
			this->position += w->parent->getRelativePosition();
			w = w->parent;
		}
	}

	/// Set parent for widget, updates the absolute position afterwards.
	inline void
	setParent(Widget* parent)
	{
		this->parent = parent;
		this->updatePosition();
	}

	/// Get dimension of widget.
	inline modm::gui::Dimension
	getDimension()
	{
		return this->dimension;
	}

	/// Get width of widget. Shortcut for getDimension().width
	inline int16_t
	getWidth()
	{
		return this->dimension.width;
	}

	/// Get height of widget. Shortcut for getDimension().height
	inline int16_t
	getHeight()
	{
		return this->dimension.height;
	}

	/// Whether widget needs to be redrawn or not.
	virtual bool
	isDirty()
	{
		return this->dirty;
	}

	/// Whether widget can handle input events.
	bool
	isInteractive()
	{
		return this->is_interactive;
	}

	/// Mark widget, that it doesn't need to be redrawn anymore.
	virtual void
	markDrawn()
	{
		this->dirty = false;
	}

	/// Mark widget, that it needs to be redrawn.
	virtual void
	markDirty()
	{
		this->dirty = true;
	}

	/// Set widget-specific font. Use modm::font::FontName as argument.
	virtual void
	setFont(const uint8_t *newFont)
	{
		this->font = modm::accessor::asFlash(newFont);
	}

	/// Set widget-specific font. Use pointer to Flash as argument.
	virtual void
	setFont(const modm::accessor::Flash<uint8_t> *font)
	{
		this->font = *font;
	}

	void
	setCallbackData(void* data)
	{
		this->cbData = data;
	}

public:
	/// Parent widget, NULL when there's no parent
	Widget *parent;

public:
	/// Unique id for every widget
	int16_t uid;

	/// dimensions
	Dimension dimension;

	/// where this widget is active or not (e.g. clicked)
	bool activated;

	/// arbitrary data that is passed to callbacks
	void* cbData;

	/// callbacks for activation and deactivation event
	eventCallback cb_activate, cb_deactivate;

	/// for now unused, maybe needed later for custom styles
	ColorPalette color_palette;

	/// position on screen (used for rendering), may be recomputed based on
	/// relative position
	modm::glcd::Point position;

	/// relative position inside a WidgetGroup for exemple
	modm::glcd::Point relative_position;

	/// has changes to be drawed
	bool dirty;

	/// whether widget will receive events
	bool is_interactive;

	/// widget specific font
	modm::accessor::Flash<uint8_t> font;

	/// list of widgets that intersect with this widget
	WidgetContainer intersecting_widgets;
};

/**
 * @ingroup modm_ui_gui
 * @author	Daniel Krebs
 */
class WidgetGroup : public Widget
{
public:
	WidgetGroup(Dimension d) :
		Widget(d, true),
		widgets()
	{
	}

	/// packs widgets inside WidgetGroup (relative coordinates!)
	bool
	pack(Widget* w, const modm::glcd::Point &coord);

	void
	render(View* view);

	bool
	handleInputEvent(const InputEvent* ev);

	void
	setColorPalette(ColorPalette& cb);

	void
	setPosition(const modm::glcd::Point& pos);

	bool
	isDirty()
	{
		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
		{
			if((*iter)->isDirty())
				return true;
		}
		return false;
	}

	void
	markDirty()
	{
		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
		{
			(*iter)->markDirty();
		}
	}

	void
	markDrawn()
	{
		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
		{
			(*iter)->markDrawn();
		}
	}

	void
	updatePosition()
	{
		this->position = this->getRelativePosition();

		Widget* w = this;

		while(w->parent != NULL)
		{
			this->position += w->parent->getRelativePosition();
			w = w->parent;
		}

		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
		{
			(*iter)->updatePosition();
		}
	}

protected:
	WidgetContainer widgets;
};

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_WIDGET_HPP
