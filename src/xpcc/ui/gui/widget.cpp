#ifndef WIDGET_CPP_
#define WIDGET_CPP_

#include "widget.hpp"

void xpcc::gui::ButtonWidget::draw(View* view)
{

	if(view == NULL)
		return;

	int16_t x, y, width, height;

	// output device of view
	xpcc::GraphicDisplay *out = view->display;

	// color palette of view
	ColorPalette *cp = this->color_palette;

	// position and dimensions
	x = this->position.x;
	y = this->position.y;
	width	= this->getWidth();
	height	= this->getHeight();

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
	out->setColor(cp[Color::TEXT]);
	out->setCursor(x + 5, y + (this->getHeight() / 2));
	*out << this->label;

}

#endif /* WIDGET_CPP_ */
