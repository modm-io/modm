#ifndef WIDGET_HPP_
#define WIDGET_HPP_

#include <xpcc/ui/display/graphic_display.hpp>
#include <xpcc/ui/gui/colorpalette.hpp>
#include <xpcc/ui/gui/types.hpp>
#include <xpcc/ui/gui/view.hpp>
#include <xpcc/debug/logger.hpp>


namespace xpcc {

namespace gui {

	static int16_t uid_global = 0;

	class Widget {

	public:

		Widget() :
			dimension(Dimension(0,0)),
			activated(false),
			cb_activate(NULL),
			cb_deactivate(NULL),
			color_palette(NULL),
			position(xpcc::glcd::Point(0,0))
		{
			// assign unique id
			uid = uid_global++;
		}

		virtual
		~Widget() {}

		virtual void
		draw(View* view) = 0;

		virtual void
		activate(const InputEvent& ev, void* data) {

			this->activated = true;

			// call callback when set
			if(cb_activate != NULL)
				this->cb_activate(ev, this, data);
		};

		virtual void
		deactivate(const InputEvent& ev, void* data) {

			this->activated = false;

			// call callback when set
			if(cb_deactivate != NULL)
				this->cb_deactivate(ev, this, data);
		};

		ColorPalette*
		getColorPalette()
		{
			return this->color_palette;
		}

		virtual void
		setColorPalette(ColorPalette* cb)
		{
			this->color_palette = cb;
		}

		virtual void
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
			return this->dimension.width;
		}

		uint16_t
		getHeight()
		{
			return this->dimension.height;
		}

	public:
		// Unique id for every widget
		int16_t uid;

		// dimensions
		Dimension dimension;

		// where this widget is active or not (e.g. clicked)
		bool activated;

		// callbacks for activation and deactivation event
		eventCallback cb_activate, cb_deactivate;

		// for now unused, maybe needed later for custom styles
		ColorPalette *color_palette;

		// position on screen (absolute)
		xpcc::glcd::Point position;

	};


	class WidgetGroup : public Widget {

	public:

		WidgetGroup(Dimension d)
		{
			this->dimension = d;
		}

		// packs widgets inside WidgetGroup (relative coordinates!)
		bool
		pack(Widget* w, const xpcc::glcd::Point &coord);

		void
		draw(View* view);

		void
		setColorPalette(ColorPalette* cb);

		void
		setPosition(const xpcc::glcd::Point& pos);

	private:
		WidgetContainer widgets;
	};

	class IntegerField : public Widget {

	public:

		IntegerField(int16_t default_value, Dimension d) :
			value(default_value)
		{
			this->dimension = d;
		}

		void
		draw(View* view);

		void
		setValue(int16_t value)
		{
			this->value = value;
		}

	private:
		int16_t value;
	};

	class ArrowButton : public Widget {

	public:
		ArrowButton(bool orientation, Dimension d) :
			orientation(orientation)
		{
			this->dimension = d;
		}

		void
		draw(View* view);

	private:
		// Orientation of the arrow. false: left | true: right
		bool orientation;


	};

	class IntegerRocker : public WidgetGroup {

	public:

		IntegerRocker(int16_t default_value, int16_t step, Dimension d) :
			WidgetGroup(d),
			value(default_value),
			step(step),
			button_increase(true, xpcc::gui::Dimension(d.height, d.height)),
			button_decrease(false, xpcc::gui::Dimension(d.height, d.height)),
			int_field(default_value, xpcc::gui::Dimension(d.width - 2*d.height, d.height))
		{
			this->dimension = d;

			button_decrease.cb_activate = &decrease_cb;
			button_increase.cb_activate = &increase_cb;

			this->pack(&button_decrease, xpcc::glcd::Point(0,0));
			this->pack(&button_increase, xpcc::glcd::Point(d.width - d.height,0));
			this->pack(&int_field, xpcc::glcd::Point(d.height, 0));

		}

		static void
		increase_cb(const xpcc::gui::InputEvent& ev, xpcc::gui::Widget* w, void* data);

		static void
		decrease_cb(const xpcc::gui::InputEvent& ev, xpcc::gui::Widget* w, void* data);

		void
		increase();

		void
		decrease();

		void
		activate(const InputEvent& ev, void* data);

		void
		deactivate(const InputEvent& ev, void* data);

		void
		handleInputEvent(const InputEvent& ev);

	private:
		int16_t value;
		int16_t step;
		ArrowButton button_decrease, button_increase;
		IntegerField int_field;

	};

	class ButtonWidget : public Widget {

	public:

		ButtonWidget(char* lbl, Dimension d) :
			label(lbl)
		{
			this->dimension = d;
		}

		ButtonWidget(char* lbl, uint16_t width, uint16_t height) :
			label(lbl)
		{
			this->dimension.width = width;
			this->dimension.height = height;
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
