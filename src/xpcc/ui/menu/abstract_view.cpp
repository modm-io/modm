/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010-2011, 2013, Georgi Grinshpun
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "view_stack.hpp"
#include "abstract_view.hpp"

// ----------------------------------------------------------------------------
xpcc::AbstractView::AbstractView(xpcc::ViewStack* stack, uint8_t identifier) :
	stack(stack), identifier(identifier), alive(true)
{
}

xpcc::AbstractView::~AbstractView()
{
}

// ----------------------------------------------------------------------------
void
xpcc::AbstractView::update()
{
	//nothing to be done
}

// ----------------------------------------------------------------------------
void
xpcc::AbstractView::shortButtonPress(xpcc::MenuButtons::Button /*button*/)
{
	//nothing to be done
}

// ----------------------------------------------------------------------------
bool
xpcc::AbstractView::isAlive() const
{
	return this->alive;
}

void
xpcc::AbstractView::remove()
{
	this->alive = false;
}

void
xpcc::AbstractView::onRemove()
{
	//nothing to be done here
}

// ----------------------------------------------------------------------------

xpcc::GraphicDisplay&
xpcc::AbstractView::display()
{
	return stack->getDisplay();
}
