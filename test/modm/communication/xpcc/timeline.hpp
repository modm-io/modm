/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012-2013, 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef TIMELINE_HPP
#define TIMELINE_HPP

#include <modm/communication/xpcc.hpp>
#include <modm/container/linked_list.hpp>
#include <modm/container/smart_pointer.hpp>

class Timeline
{
public:
	enum Type
	{
		EVENT,
		ACTION,
		RESPONSE
	};
	
	struct Event
	{
		Event(Type type, uint8_t component, uint8_t id, uint8_t source);
		
		template <typename T>
		Event(Type type, uint8_t component, uint8_t id, uint8_t source,
				const T *parameter);
		
		Event(const Event& other);
		
		Type type;
		uint8_t component;
		uint8_t id;
		uint8_t source;
		
		modm::SmartPointer payload;
		
	private:
		Event&
		operator = (const Event& other);
	};
	
	modm::LinkedList<Event> events;
};

template <typename T>
Timeline::Event::Event(Type type, uint8_t component, uint8_t id, uint8_t source,
		const T *parameter) :
	type(type),
	component(component),
	id(id),
	source(source),
	payload(parameter)
{
}

#endif	// TIMELINE_HPP
