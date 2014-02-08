#ifndef VIEW_HPP_
#define VIEW_HPP_

#include <xpcc/ui/display/graphic_display.hpp>
#include <xpcc/processing/protothread.hpp>
#include "types.hpp"
#include "colorpalette.hpp"
#include "widget.hpp"

namespace xpcc {

namespace gui {

	class View : xpcc::pt::Protothread {

	public:

		// initialize with 320x240 resolution for now
		View(xpcc::GraphicDisplay *display, ColorPalette cp[], inputQueue *input_queue) :
			display(display),
			color_palette(cp),
			input_queue(input_queue),
			canvas_x(320), canvas_y(240)
		{
		}

		// Protothread main loop. Everything dynamic happens here!
		bool
		run();

		// add widget to view
		bool
		pack(Widget *w, const xpcc::glcd::Point &coord);

		// draw view on output device
		void
		render();

		// set color palette for view and every contained widget
		void
		setColorPalette(ColorPalette *cp);

		ColorPalette*
		getColorPalette()
		{
			return this->color_palette;
		}

		void
		setDisplay(xpcc::GraphicDisplay *display)
		{
			this->display = display;
		}


	public:
		WidgetContainer widgets;
		xpcc::GraphicDisplay *display;
		ColorPalette *color_palette;
		inputQueue *input_queue;

	private:
		uint16_t canvas_x, canvas_y;

	};

}
}

#endif /* VIEW_HPP_ */
