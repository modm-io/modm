/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010-2011, 2013, Georgi Grinshpun
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Niklas Hauser
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
