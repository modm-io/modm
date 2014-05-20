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
#include "abstract_view.hpp"

// ----------------------------------------------------------------------------
xpcc::gui::AbstractView::AbstractView(xpcc::gui::ViewStack* stack, uint8_t identifier, xpcc::gui::Dimension dimension) :
	stack(stack),
	identifier(identifier),
	alive(true),
	dimension(dimension),
	colorpalette(DEFAULT_COLORPALETTE)

{
}

// ----------------------------------------------------------------------------
xpcc::gui::AbstractView::~AbstractView()
{
}

// ----------------------------------------------------------------------------
void
xpcc::gui::AbstractView::update()
{

	InputEvent* ev;

	xpcc::gui::inputQueue* input_queue = this->stack->getInputQueue();

	while(!input_queue->isEmpty()) {

		// pop input event
		ev = input_queue->get();
		input_queue->pop();

		// start event handling
		for(auto iter = widgets.begin(); iter != widgets.end(); ++iter) {
			/*
			 * TODO: implement efficient algorithm here (note: segment tree)
			 */
			if((*iter)->handleInputEvent(ev))
			{
				/* we found the widget that this event was meant for*/
				break;
			}
		}

		// delete InputEvent, we don't need it anymore
		delete ev;
	}
}

// ----------------------------------------------------------------------------
void
xpcc::gui::AbstractView::shortButtonPress(xpcc::MenuButtons::Button /*button*/)
{
	//nothing to be done
}

// ----------------------------------------------------------------------------
bool
xpcc::gui::AbstractView::pack(Widget *w, const xpcc::glcd::Point &coord)
{
	if(coord.x >= this->dimension.width ||
	   coord.y >= this->dimension.height ||
	   coord.x < 0 ||
	   coord.y < 0)
	{
		return false;
	}

	w->setPosition(coord);
	w->setColorPalette(this->colorpalette);

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
bool
xpcc::gui::AbstractView::isAlive() const
{
	return this->alive;
}

// ----------------------------------------------------------------------------
void
xpcc::gui::AbstractView::remove()
{
	this->alive = false;
}

// ----------------------------------------------------------------------------
void xpcc::gui::AbstractView::draw()
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
xpcc::gui::AbstractView::onRemove()
{
	//nothing to be done here
}

// ----------------------------------------------------------------------------
xpcc::GraphicDisplay&
xpcc::gui::AbstractView::display()
{
	return stack->getDisplay();
}

// ----------------------------------------------------------------------------
void
xpcc::gui::AbstractView::setColorPalette(ColorPalette* cp)
{

	this->colorpalette = cp;

	/*
	 * widget specific color palette disabled for now, so this isn't really needed
	 */

	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		(*iter)->color_palette = cp;
	}

}
