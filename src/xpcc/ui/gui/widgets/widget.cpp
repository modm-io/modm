// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "widget.hpp"

bool xpcc::gui::WidgetGroup::pack(Widget* w, const xpcc::glcd::Point& coord)
{
	if(coord.x > (this->dimension.width - w->dimension.width ) || coord.y > (this->dimension.height - w->dimension.height) || coord.x < 0 || coord.y < 0)
		return false;

	/* Widget needs to know its parent to calculate its real, absolute
	 * position */
	w->setParent(this);

	/* Set relative position of widget inside WidgetGroup, absolute position
	 * will be calculated automatically */
	w->setPosition(coord);

	/* Add widget to WidgetGroup's list of child widgets */
	this->widgets.append(w);

	return true;
}

void xpcc::gui::WidgetGroup::render(View* view)
{
	/* draw all widgets */
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter) {
		if((*iter)->isDirty())
		{
			(*iter)->draw(view);
		}
	}
}

void xpcc::gui::WidgetGroup::setColorPalette(ColorPalette& cp)
{
	copyColorPalette(cp, this->color_palette);

	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter) {
		(*iter)->setColorPalette(cp);
	}
}

void xpcc::gui::WidgetGroup::setPosition(const xpcc::glcd::Point& pos)
{
	/* the same as in Widget::setPosition() */
	this->setRelativePosition(pos);
	this->updatePosition();

	/* We need to update the absolute positions of all child widgets */
	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter) {
		(*iter)->updatePosition();
	}
}

void xpcc::gui::Widget::updateIntersections(WidgetContainer *widgets)
{
	/* clear list, but keep size. it won't shrink unless widgets will be removed */
	this->intersecting_widgets.removeAll();

	/* check every widget if it intersects with this one */
	for(auto iter = widgets->find(this); iter != widgets->end(); ++iter)
	{
		/* don't test against yourself */
		if((*iter)->uid == this->uid)
			continue;

		if(this->checkIntersection((*iter)))
		{
			/* intersection found, so append to intersecting list */
			this->intersecting_widgets.append((*iter));
		}
	}
}

bool xpcc::gui::Widget::hasIntersections()
{
	return !this->intersecting_widgets.isEmpty();
}

bool xpcc::gui::Widget::checkIntersection(Widget* w)
{
	/* intersection with itself makes no sense */
	if(w->uid == this->uid)
		return false;

	/* coordinates of upper left and lower right corner of this widget */
	auto upper_left = this->getPosition();
	auto lower_right = xpcc::glcd::Point(upper_left.x + this->getWidth(), upper_left.y + this->getHeight());

	/* coordinates of upper left and lower right corner of the argument Widget* w */
	auto upper_left2 = w->getPosition();
	auto lower_right2 = xpcc::glcd::Point(upper_left2.x + w->getWidth(), upper_left2.y + w->getHeight());

	/* check if rectangles contituted by precedingly defined
	 * coordinates DON'T overlap
	 * */
	if(upper_left.x > lower_right2.x ||
	   upper_left2.x > lower_right.x ||
	   upper_left.y > lower_right2.y ||
	   upper_left2.y > lower_right.y
	  )
	{
		return false;
	}

	return true;
}

bool xpcc::gui::Widget::handleInputEvent(const InputEvent* ev)
{
	/* absolute position of widget */
	auto position = this->getPosition();

	/* check if event is within area */
	if((position.x < ev->coord.x) &&
	   (position.y < ev->coord.y) &&
	   ((position.x + this->getWidth()) > ev->coord.x) &&
	   ((position.y + this->getHeight()) > ev->coord.y))
	{
		/* check if widget has interaction */
		if(this->isInteractive())
		{
			if(ev->direction == xpcc::gui::InputEvent::Direction::DOWN) {
				this->activate(*ev, this->cbData);
			} else if(ev->direction == xpcc::gui::InputEvent::Direction::UP) {
				this->deactivate(*ev, this->cbData);
			}
		}
		/* event was meant for this widget, so return true regardless of interaction */
		return true;
	}

	return false;
}

bool xpcc::gui::WidgetGroup::handleInputEvent(const InputEvent* ev)
{

	for(auto iter = widgets.begin(); iter != widgets.end(); ++iter)
	{
		if((*iter)->handleInputEvent(ev))
		{
			/* event was meant for this widget, so we can break here */
			return true;
		}
	}

	return false;
}
