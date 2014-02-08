#ifndef WIDGET_HPP_
#define WIDGET_HPP_

#include <xpcc/ui/display/graphic_display.hpp>
#include <xpcc/ui/gui/colorpalette.hpp>
#include <xpcc/ui/gui/types.hpp>
#include <xpcc/ui/gui/view.hpp>


namespace xpcc {

namespace gui {

	static int16_t uid_global = 0;

	class Widget {

	public:

		Widget() :
			activated(false),
			cb_activate(NULL),
			cb_deactivate(NULL),
			color_palette(NULL),
			position(xpcc::glcd::Point(0,0))
		{
			// assign unique id
			uid = uid_global++;

			// preliminary set the dimension to standard 100x30
			width = 100;
			height = 60;
		}

		virtual
		~Widget() {}

		virtual void
		draw(View* view) = 0;

		void
		activate(const InputEvent& ev) {

			this->activated = true;

			// call callback when set
			if(cb_activate != NULL)
				this->cb_activate(ev, this);
		};

		void
		deactivate(const InputEvent& ev) {

			this->activated = false;

			// call callback when set
			if(cb_deactivate != NULL)
				this->cb_deactivate(ev, this);
		};

		void
		setPosition(const xpcc::glcd::Point& pos)
		{
			this->position = pos;
		}

		xpcc::glcd::Point
		getPosition()
		{
			return this->position;
		}

		uint16_t
		getWidth()
		{
			return this->width;
		}

		uint16_t
		getHeight()
		{
			return this->height;
		}

	public:
		// Unique id for every widget
		int16_t uid;

		// dimensions
		uint16_t width, height;

		// where this widget is active or not (e.g. clicked)
		bool activated;

		// callbacks for activation and deactivation event
		eventCallback cb_activate, cb_deactivate;

		// for now unused, maybe needed later for custom styles
		ColorPalette *color_palette;

		// position on screen (absolute)
		xpcc::glcd::Point position;

	};


	class ButtonWidget : public Widget {

	public:

		ButtonWidget(char* lbl) : label(lbl)
		{
		}

		void
		draw(View* view);

		void
		setLabel(char* lbl)
		{
			this->label = lbl;
		}

	public:
		char *label;

	};

}
}

#endif /* WIDGET_HPP_ */
