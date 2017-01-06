/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Kevin Laeufer
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

#include "view_stack.hpp"
#include "view.hpp"
#include "types.hpp"

// ----------------------------------------------------------------------------
modm::gui::View::View(modm::gui::GuiViewStack* stack, uint8_t identifier, modm::gui::Dimension dimension) :
	AbstractView(stack, identifier),
	stack(stack),
	dimension(dimension)
{
	this->display().clear();
}

// ----------------------------------------------------------------------------
modm::gui::View::~View()
{
}

// ----------------------------------------------------------------------------
void
modm::gui::View::update()
{
	preUpdate();

	InputEvent* ev;

	modm::gui::inputQueue* input_queue = this->stack->getInputQueue();

	while(!input_queue->isEmpty()) {

		// pop input event
		ev = input_queue->get();
		input_queue->pop();

		/* Start event handling, go through array in reverse order because
		 * later added (higher index) widgets will be rendered on top of
		 * the other widgets, so they will also catch the input event with
		 * higher precedence. */
		for(int16_t i = widgets.getSize()-1; i >= 0; i--) {
			/*
			 * TODO: implement efficient algorithm here (note: segment tree)
			 */
			if((widgets[i])->handleInputEvent(ev))
			{
				/* we found the widget that this event was meant for*/
				break;
			}
		}

		// delete InputEvent, we don't need it anymore
		delete ev;
	}

	postUpdate();
}

// ----------------------------------------------------------------------------
bool
modm::gui::View::pack(Widget *w, const modm::glcd::Point &coord)
{
	if(coord.x >= this->dimension.width ||
	   coord.y >= this->dimension.height ||
	   coord.x < 0 ||
	   coord.y < 0)
	{
		return false;
	}

	w->setPosition(coord);
//	w->setColorPalette(this->colorpalette);

	this->widgets.append(w);

	/* Check if there are other widgets on top of this one, 'on top' means
	 * that they were 'packed' later, so they are located at an higher index
	 * in the WidgetContainer this->widgets
	 * */
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		// TODO:
		// this is really ugly and may have quite some perfomance impact when
		// there are a lot of widgets
		(*iter)->updateIntersections(&(this->widgets));
	}

	return true;
}

// ----------------------------------------------------------------------------
void
modm::gui::View::remove()
{
	this->alive = false;
}

// ----------------------------------------------------------------------------
void modm::gui::View::draw()
{
//	auto display = &this->stack->getDisplay();

	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		/* Only redraw when widget is dirty */
		if((*iter)->isDirty())
		{
			/* draw widget
			 * redrawing widgets on top, that would be overdrawn, is handled
			 * inside Widget::draw()
			 * */
			(*iter)->draw(this);
		}
	}
}

// ----------------------------------------------------------------------------
void
modm::gui::View::setColorPalette(ColorPalette& cp)
{
	this->colorpalette = cp;
	this->display().setBackgroundColor(cp.getColor(Color::BACKGROUND));

	/*
	 * widget specific color palette disabled for now, so this isn't really needed
	 */

	this->display().clear();

	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		(*iter)->setColorPalette(cp);
		(*iter)->markDirty();
	}
}

void modm::gui::View::markDirty()
{
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		(*iter)->markDirty();
	}
}

void modm::gui::View::markDrawn()
{
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		(*iter)->markDrawn();
	}
}
