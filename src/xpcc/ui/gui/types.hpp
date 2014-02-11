#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <xpcc/container/dynamic_array.hpp>
#include <xpcc/container/linked_list.hpp>
#include <xpcc/container/queue.hpp>

namespace xpcc {

namespace gui {

	// forward declare so that everything compiles. can this be done better?
	class View;
	class Widget;


	// Container used in view to store widgets
	typedef xpcc::DynamicArray<Widget*> WidgetContainer;


	// Input event that is collected when some input happens. Will be proccessed by View
	struct InputEvent {

		enum class Type {
			BUTTON,
			TOUCH
		};

		enum class Direction {
			UP,
			DOWN
		};

		Type type;

		xpcc::glcd::Point coord;
	};

	typedef xpcc::Queue<InputEvent, xpcc::LinkedList<InputEvent> > inputQueue;


	// Callback when an event happend
	typedef void (*eventCallback)(const InputEvent&, Widget*);

	typedef struct Dimension {
		Dimension(uint16_t w, uint16_t h) : width(w), height(h) {}
		uint16_t width, height;
	} Dimension;

}
}

#endif /* TYPES_HPP_ */
