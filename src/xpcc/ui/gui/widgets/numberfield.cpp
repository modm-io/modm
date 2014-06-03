#include "numberfield.hpp"

xpcc::gui::FloatField::FloatField(float value, Dimension d) :
	NumberField<float>(value, d)
{
}

void xpcc::gui::FloatField::render(AbstractView* view)
{
	if(view == 0)
		return;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

	// color palette of view
	ColorPalette cp = this->color_palette;

	const uint16_t box_width = this->dimension.width - 4;
	const uint16_t box_height = this->dimension.height;
	const uint16_t box_x = this->getPosition().x + 2;
	const uint16_t box_y = this->getPosition().y;

	// clear background
	out->setColor(cp[Color::BACKGROUND]);
	out->fillRectangle(this->getPosition(), this->getWidth(), this->getHeight());

	// draw box
	out->setColor(cp[Color::BORDER]);
	out->drawLine(box_x, box_y, box_x + box_width - 1, box_y);
	out->drawLine(box_x, box_y, box_x, box_y + box_height - 1);
	out->drawLine(box_x + box_width - 1, box_y + box_height - 1, box_x + box_width - 1, box_y);
	out->drawLine(box_x + box_width, box_y + box_height - 1, box_x, box_y + box_height - 1);

	// draw number
	const uint16_t stringHeight = out->getFontHeight();

	out->setColor(cp[Color::TEXT]);
	out->setCursor(box_x + 10, box_y + (box_height - stringHeight) / 2);

	int32_t beforeComma = static_cast<int32_t>(this->getValue());
	uint32_t afterComma = static_cast<uint32_t>((this->getValue() - beforeComma) * 1000);

	*out << beforeComma << ".";
	out->printf("%03ld", afterComma);

//	*out << this->value;
}
