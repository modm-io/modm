#ifndef WIDGET_CPP_
#define WIDGET_CPP_

#include "widget.hpp"
#include <xpcc/debug/logger.hpp>

void xpcc::gui::ButtonWidget::draw(View* view)
{

	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay *out = view->display;

	// color palette of view
	ColorPalette *cp = this->color_palette;

	// position and dimensions
	const uint16_t x = this->position.x;
	const uint16_t y = this->position.y;
	const uint16_t width = this->getWidth();
	const uint16_t height = this->getHeight();

	/*
	 * choose color depending on button state
	 */
	if(this->activated)
		out->setColor(cp[Color::ACTIVATED]);
	else
		out->setColor(cp[Color::DEACTIVATED]);


	/*
	 * draw button outline
	 */
	out->drawLine(x, y, x + width, y);
	out->drawLine(x, y, x, y + height);
	out->drawLine(x + width, y + height, x + width, y);
	out->drawLine(x + width, y + height, x, y + height);

	/*
	 * draw button text
	 * TODO: center text
	 */

	const uint16_t stringWidth = out->getStringWidth(this->label);
	const uint16_t stringHeight = out->getFontHeight();

	out->setColor(cp[Color::TEXT]);
	out->setCursor(x + (width - stringWidth) / 2, y + (height - stringHeight) / 2);
	*out << this->label;

}
/*
void xpcc::gui::IntegerRocker::draw(View* view)
{
}
*/
void xpcc::gui::ArrowButton::draw(View* view)
{
	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay *out = view->display;

	// color palette of view
	ColorPalette *cp = this->color_palette;

	// position and dimensions
	const uint16_t x = this->position.x;
	const uint16_t y = this->position.y;
	const uint16_t width = this->getWidth();
	const uint16_t height = this->getHeight();


	// calculations for arraw
	const uint16_t arrow_padding = 6;
	const uint16_t arrow_height = height - arrow_padding*2;		// vertical padding
	const uint16_t arrow_width = width - arrow_padding*2;		// horizontal padding
	const uint16_t arrow_x = x + arrow_padding;
	const uint16_t arrow_y = y + arrow_padding;


	// draw arrow
	out->setColor(cp[Color::TEXT]);

	if(this->orientation == true) {
		out->drawLine(arrow_x, arrow_y, arrow_x, arrow_y + arrow_height);
		out->drawLine(arrow_x, arrow_y, arrow_x + arrow_width, arrow_y + arrow_height/2);
		out->drawLine(arrow_x, arrow_y + arrow_height, arrow_x + arrow_width, arrow_y + arrow_height/2);
	} else {
		out->drawLine(arrow_x + arrow_width, arrow_y, arrow_x + arrow_width, arrow_y + arrow_height);
		out->drawLine(arrow_x, arrow_y + arrow_height/2, arrow_x + arrow_width, arrow_y);
		out->drawLine(arrow_x, arrow_y + arrow_height/2, arrow_x + arrow_width, arrow_y + arrow_height);
	}


	if(this->activated)
		out->setColor(cp[Color::ACTIVATED]);
	else
		out->setColor(cp[Color::BORDER]);

	// draw box
	//out->setColor(cp[Color::BORDER]);
	out->drawLine(x, y, x + width, y);
	out->drawLine(x, y, x, y + height);
	out->drawLine(x + width, y + height, x + width, y);
	out->drawLine(x + width, y + height, x, y + height);

}

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

void xpcc::gui::WidgetGroup::draw(View* view)
{
	// draw all widgets
	WidgetContainer::iterator it;

	for(it = widgets.begin(); it != widgets.end(); ++it) {
		(*it)->draw(view);
	}
}

void xpcc::gui::IntegerRocker::increase()
{
	this->value += this->step;
	this->int_field.setValue(this->value);
}

void xpcc::gui::IntegerRocker::decrease()
{
	this->value -= this->step;
	this->int_field.setValue(this->value);
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

void xpcc::gui::IntegerRocker::activate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(ev);
}

void xpcc::gui::IntegerRocker::deactivate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(ev);
}

void xpcc::gui::IntegerRocker::increase_cb(const xpcc::gui::InputEvent& ev,
		xpcc::gui::Widget* w, void* data)
{
	(void) ev;
	(void) w;

	IntegerRocker *rocker = static_cast<IntegerRocker*>(data);

	rocker->increase();
}

void xpcc::gui::IntegerRocker::decrease_cb(const xpcc::gui::InputEvent& ev,
		xpcc::gui::Widget* w, void* data)
{
	(void) ev;
	(void) w;

	IntegerRocker *rocker = static_cast<IntegerRocker*>(data);

	rocker->decrease();
}

void xpcc::gui::IntegerRocker::handleInputEvent(const InputEvent& ev)
{
	// convert to relative coordinate
	uint16_t ev_x = ev.coord.x - this->position.x;

	Widget* w = 0;


	if(ev_x < this->dimension.height) {
		w = &button_decrease;
	} else if(ev_x > (this->dimension.width - this->dimension.height)) {
		w = &button_increase;
	} else {
		// text field, do nothing
	}
	if(!w)
		return;

	if(ev.direction == xpcc::gui::InputEvent::Direction::UP) {
		w->deactivate(ev, this);
	} else if(ev.direction == xpcc::gui::InputEvent::Direction::DOWN) {
		w->activate(ev, this);
	}
}

void xpcc::gui::IntegerField::draw(View* view)
{

	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay *out = view->display;

	// color palette of view
	ColorPalette *cp = this->color_palette;

	const uint16_t box_width = this->dimension.width - 4;
	const uint16_t box_height = this->dimension.height;
	const uint16_t box_x = this->position.x + 2;
	const uint16_t box_y = this->position.y;

	// draw box
	out->setColor(cp[Color::BORDER]);
	out->drawLine(box_x, box_y, box_x, box_y + box_height);
	out->drawLine(box_x, box_y, box_x + box_width, box_y);
	out->drawLine(box_x, box_y + box_height, box_x + box_width, box_y + box_height);
	out->drawLine(box_x + box_width, box_y, box_x + box_width, box_y + box_height);

	// draw number
	const uint16_t stringHeight = out->getFontHeight();

	out->setColor(cp[Color::TEXT]);
	out->setCursor(box_x + 10, box_y + (box_height - stringHeight) / 2);
	*out << this->value;

}

#endif /* WIDGET_CPP_ */
