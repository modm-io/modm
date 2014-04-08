#include "integerfield.hpp"

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
