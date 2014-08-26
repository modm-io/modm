// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
	colorpalette(DEFAULT_COLORPALETTE)

{
	this->setColorPalette(DEFAULT_COLORPALETTE);
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

	copyColorPalette(cp, this->colorpalette);
	this->display().setBackgroundColor(cp[Color::BACKGROUND]);

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
