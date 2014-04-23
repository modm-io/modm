#include "widget.hpp"

bool xpcc::gui::WidgetGroup::pack(Widget* w, const xpcc::glcd::Point& coord)
{
	if(coord.x > (this->dimension.width - w->dimension.width ) || coord.y > (this->dimension.height - w->dimension.height) || coord.x < 0 || coord.y < 0)
		return false;

	// preliminary set relative position. will be translated to absolute coordinates
	// when WidgetGroup will be packed to a view
	w->setPosition(coord);

	this->widgets.append(w);

	return true;
}

void xpcc::gui::WidgetGroup::draw(AbstractView* view)
{
	// draw all widgets
	WidgetContainer::iterator it;

	for(it = widgets.begin(); it != widgets.end(); ++it) {
		(*it)->draw(view);
	}
}

void xpcc::gui::WidgetGroup::setColorPalette(ColorPalette* cb)
{
	this->color_palette = cb;

	WidgetContainer::iterator it;

	for(it = widgets.begin(); it != widgets.end(); ++it) {
		(*it)->setColorPalette(cb);
	}
}

void xpcc::gui::WidgetGroup::setPosition(const xpcc::glcd::Point& pos)
{
	this->position = pos;

	WidgetContainer::iterator it;
	for(it = widgets.begin(); it != widgets.end(); ++it) {

		xpcc::glcd::Point absolute_coord;

		absolute_coord.x = pos.x + (*it)->position.x;
		absolute_coord.y = pos.y + (*it)->position.y;

		(*it)->setPosition(absolute_coord);
	}
}
