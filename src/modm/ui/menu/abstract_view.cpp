/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010-2011, 2013, Georgi Grinshpun
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Thorsten Lajewski
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
modm::AbstractView::AbstractView(modm::ViewStack* stack, uint8_t identifier) :
	stack(stack), identifier(identifier), alive(true)
{
}

modm::AbstractView::~AbstractView()
{
}

// ----------------------------------------------------------------------------
void
modm::AbstractView::update()
{
	//nothing to be done
}

// ----------------------------------------------------------------------------
void
modm::AbstractView::shortButtonPress(modm::MenuButtons::Button /*button*/)
{
	//nothing to be done
}

// ----------------------------------------------------------------------------
bool
modm::AbstractView::isAlive() const
{
	return this->alive;
}

void
modm::AbstractView::remove()
{
	this->alive = false;
}

void
modm::AbstractView::onRemove()
{
	//nothing to be done here
}

// ----------------------------------------------------------------------------

modm::GraphicDisplay&
modm::AbstractView::display()
{
	return stack->getDisplay();
}
