#include <xpcc/ui/display/graphic_display.hpp>
#include "view.hpp"

bool
xpcc::gui::View::pack(Widget *w, const xpcc::glcd::Point &coord)
{
	if(coord.x >= canvas_x || coord.y >= canvas_y || coord.x < 0 || coord.y < 0)
		return false;

	w->setPosition(coord);
	w->color_palette = this->color_palette;

	this->widgets.append(w);

	return true;
}

bool
xpcc::gui::View::run()
{
	PT_BEGIN();

	// render the view for the first time
	this->render();

	while(true) {

		// stop if no event occured
		PT_WAIT_UNTIL(!this->input_queue->isEmpty());

		// pop input event
		InputEvent ev = this->input_queue->get();
		this->input_queue->pop();


		// start event handling
		xpcc::glcd::Point position;
		WidgetContainer::iterator it;

		for(it = widgets.begin(); it != widgets.end(); ++it) {
			/*
			 * TODO: implement efficient algorithm here (note: segment tree)
			 */

			position = (*it)->getPosition();

			if((position.x < ev.coord.x) && ((position.x + (*it)->getWidth())  > ev.coord.x) &&
			   (position.y < ev.coord.y) && ((position.y + (*it)->getHeight()) > ev.coord.y))
			{
				(*it)->activate(ev);
			} else {
				(*it)->deactivate(ev);
			}
		}

		// draw view
		this->render();

	}

	PT_END();
}

void
xpcc::gui::View::render()
{
	WidgetContainer::iterator it;

	// clear screen
	display->setBackgroundColor(this->color_palette[Color::BACKGROUND]);
	display->clear();

	unsigned int i = 0;
	for(i = 0; i < widgets.getSize(); i++)
	{
		// iterate over all widgets and draw each
		widgets[i]->draw(this);
	}

}

void
xpcc::gui::View::setColorPalette(ColorPalette* cp)
{

	this->color_palette = cp;


	/*
	 * widget specific color palette disabled for now, so this isn't really needed
	 */
	unsigned int i = 0;

	for(i = 0; i < widgets.getSize(); i++) {
		widgets[i]->color_palette = cp;
	}

}
