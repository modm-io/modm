// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "view_stack.hpp"

// ----------------------------------------------------------------------------
xpcc::gui::GuiViewStack::GuiViewStack(xpcc::GraphicDisplay* display, xpcc::gui::inputQueue* queue) :
	ViewStack(display),
	input_queue(queue)
{
}

// ----------------------------------------------------------------------------
xpcc::gui::GuiViewStack::~GuiViewStack()
{
}

// ----------------------------------------------------------------------------
void
xpcc::gui::GuiViewStack::pop()
{
	xpcc::gui::View *topElement = this->stack.get();
	this->stack.pop();

	delete topElement;
}

// ----------------------------------------------------------------------------
void
xpcc::gui::GuiViewStack::update()
{
	xpcc::gui::View* top = this->get();

	if(top == NULL)
		return;

	top->update();
	if (top->isAlive())
	{
		if (top->hasChanged())
		{
			top->draw();
			this->display->update();
		}
	}
	else
	{
		// Remove old view
		top->onRemove();
		this->pop();

		// Get new screen
		top = this->get();
		top->markDirty();	// Only difference to xpcc::ViewStack::update()
		top->update();
		this->display->clear();
		top->draw();
		this->display->update();
	}
}
