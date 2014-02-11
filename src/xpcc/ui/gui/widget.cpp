#ifndef WIDGET_CPP_
#define WIDGET_CPP_

#include "widget.hpp"

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

#endif /* WIDGET_CPP_ */
