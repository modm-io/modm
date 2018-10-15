/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "flag.hpp"

modm::atomic::Flag::Flag(bool inState) :
	state(inState)
{
}

modm::atomic::Flag::Flag(const Flag& other) :
	state(other.state)
{
}

modm::atomic::Flag&
modm::atomic::Flag::operator = (const Flag& other)
{
	this->state = other.state;
	return *this;
}
