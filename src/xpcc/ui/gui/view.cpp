// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "view_stack.hpp"
#include "view.hpp"
#include "types.hpp"

// ----------------------------------------------------------------------------
xpcc::gui::View::View(xpcc::gui::GuiViewStack* stack, uint8_t identifier, xpcc::gui::Dimension dimension) :
	AbstractView(stack, identifier),
	stack(stack),
	dimension(dimension),
	colorpalette(palette_colors)
{
	this->display().clear();
}

// ----------------------------------------------------------------------------
xpcc::gui::View::~View()
{
}

// ----------------------------------------------------------------------------
void
xpcc::gui::View::update()
{
	preUpdate();

	InputEvent* ev;

	xpcc::gui::inputQueue* input_queue = this->stack->getInputQueue();

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
xpcc::gui::View::pack(Widget *w, const xpcc::glcd::Point &coord)
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
xpcc::gui::View::remove()
{
	this->alive = false;
}

// ----------------------------------------------------------------------------
void xpcc::gui::View::draw()
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
xpcc::gui::View::setColorPalette(ColorPalette& cp)
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

void xpcc::gui::View::markDirty()
{
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		(*iter)->markDirty();
	}
}

void xpcc::gui::View::markDrawn()
{
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		(*iter)->markDrawn();
	}
}
