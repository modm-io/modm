/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010-2011, 2013, Georgi Grinshpun
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
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
modm::ViewStack::ViewStack(modm::GraphicDisplay* display) :
	display(display)
{
}

// ----------------------------------------------------------------------------
modm::ViewStack::~ViewStack()
{
}

// ----------------------------------------------------------------------------
void
modm::ViewStack::pop()
{
	modm::AbstractView *topElement = this->stack.get();
	this->stack.pop();
	
	delete topElement;
}

// ----------------------------------------------------------------------------
void
modm::ViewStack::update()
{
	modm::AbstractView* top = this->get();

	if (top == NULL)
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
		top->update();
		this->display->clear();
		top->draw();
		this->display->update();
	}
}

// ----------------------------------------------------------------------------
void
modm::ViewStack::shortButtonPress(modm::MenuButtons::Button button)
{
	modm::AbstractView* top = this->get();
	top->shortButtonPress(button);
}
