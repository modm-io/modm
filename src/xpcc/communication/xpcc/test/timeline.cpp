/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstring>	// for std::memcpy
#include "timeline.hpp"

Timeline::Event::Event(Type type, uint8_t component, uint8_t id, 
		uint8_t source) :
	type(type),
	component(component),
	id(id),
	source(source),
	payload()
{
}

Timeline::Event::Event(const Event& other) :
	type(other.type),
	component(other.component),
	id(other.id),
	source(other.source),
	payload(other.payload)
{
}
