#ifndef TYPES_HPP_
#define TYPES_HPP_

#include <xpcc/ui/display.hpp>
#include <xpcc/container/dynamic_array.hpp>
#include <xpcc/container/linked_list.hpp>
#include <xpcc/container/queue.hpp>
#include <xpcc/processing.hpp>

namespace xpcc {

namespace gui {

	// forward declare so that everything compiles. can this be done better?
	class View;
	class Widget;


	// Container used in view to store widgets
	typedef xpcc::DynamicArray<Widget*> WidgetContainer;

	typedef void (*genericCallback)(void*);

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
		Direction direction;

		xpcc::glcd::Point coord;
	};

	class AsyncEvent
	{
	public:
		AsyncEvent(uint16_t delay_ms, genericCallback cb, void* cb_data) :
			delay_ms(delay_ms),
			cb(cb),
			cb_data(cb_data),
			timeout(delay_ms),
			already_run(false)
		{

		}

		bool is_expired()
		{
			if(timeout.isExpired()) {
				if(!already_run && ( cb != NULL )) {
					already_run = true;
					cb(cb_data);
				}

				return true;

			} else {

				return false;

			}
		}

	private:
		uint16_t delay_ms;
		genericCallback cb;
		void* cb_data;
		xpcc::Timeout<> timeout;
		bool already_run;
	};

	typedef xpcc::Queue<InputEvent, xpcc::LinkedList<InputEvent> > inputQueue;

	// Callback when an event happend
	typedef void (*eventCallback)(const InputEvent&, Widget*, void*);

	typedef struct Dimension {
		Dimension(uint16_t w, uint16_t h) : width(w), height(h) {}
		uint16_t width, height;
	} Dimension;

}
}

#endif /* TYPES_HPP_ */
