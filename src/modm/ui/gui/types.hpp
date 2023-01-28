/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014-2015, 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_GUI_TYPES_HPP
#define MODM_GUI_TYPES_HPP

#include <modm/ui/display.hpp>
#include <modm/container/dynamic_array.hpp>
#include <modm/container/linked_list.hpp>
#include <modm/container/queue.hpp>
#include <modm/container/doubly_linked_list.hpp>
#include <modm/processing/timer.hpp>

#include <modm/debug/logger.hpp>

/* Eclipse needs this to resolve NULL*/
#ifndef NULL
#define NULL 0
#endif

namespace modm
{

namespace gui
{

// forward declare so that everything compiles. can this be done better?
class View;
class Widget;


/// Container used in view to store widgets
/// @ingroup modm_ui_gui
typedef modm::DynamicArray<Widget*> WidgetContainer;
/// @ingroup modm_ui_gui
typedef void (*genericCallback)(void*);

/**
 * Input event that is collected when some input happens. Will be processed by View
 *
 * @author	Daniel Krebs
 * @ingroup modm_ui_gui
 */
class InputEvent
{
public:

	enum class
	Type {
		BUTTON,
		TOUCH
	};

	enum class
	Direction {
		UP,
		DOWN
	};

	InputEvent(modm::glcd::Point point, Type type, Direction direction) :
		type(type),
		direction(direction),
		coord(point)
	{
	}

	InputEvent()
	{
	}

	Type type;
	Direction direction;

	modm::glcd::Point coord;
};

/**
 * Asynchronous Event
 *
 * is_expired() must be called repeatedly for the callback to get called. It is
 * garantueed that the callback will only be called once.
 *
 * @author	Daniel Krebs
 * @ingroup modm_ui_gui
 */
class AsyncEvent
{
public:
	AsyncEvent(uint16_t delayMilliseconds, genericCallback cb, void* cb_data) :
		cb(cb),
		cb_data(cb_data),
		timeout(std::chrono::milliseconds(delayMilliseconds)),
		already_run(false)
	{
	}

	bool is_expired()
	{
		if(timeout.isExpired())
		{
			if(!already_run && ( cb != NULL ))
			{
				already_run = true;
				cb(cb_data);
			}
			return true;
		}
		else {
			return false;
		}
	}

private:
	genericCallback cb;
	void* cb_data;
	modm::ShortTimeout timeout;
	bool already_run;
};

/// @ingroup modm_ui_gui
typedef modm::DoublyLinkedList<AsyncEvent*> AsyncEventList;
/// @ingroup modm_ui_gui
typedef modm::Queue<InputEvent*, modm::LinkedList<InputEvent*> > inputQueue;

/// Callback when an event happend
/// @ingroup modm_ui_gui
typedef void (*eventCallback)(const InputEvent&, Widget*, void*);
/// @ingroup modm_ui_gui
typedef struct Dimension
{
	Dimension(int16_t w, int16_t h) :
		width(w), height(h)
	{
	}

	int16_t width, height;
	bool operator == (const Dimension& other) const
	{
		return (width == other.width) && (height == other.height);
	}

} Dimension;
/// @ingroup modm_ui_gui
typedef modm::glcd::Point Point;

}	// namespace gui

}	// namespace modm

#endif  // MODM_GUI_TYPES_HPP
