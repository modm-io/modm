#include "button.hpp"

void xpcc::gui::ButtonWidget::render(AbstractView* view)
{

	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

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
	out->drawLine(x, y, x + width - 1, y);
	out->drawLine(x, y, x, y + height - 1);
	out->drawLine(x + width - 1, y + height - 1, x + width - 1, y);
	out->drawLine(x + width, y + height - 1, x, y + height - 1);

	/*
	 * draw button text
	 * TODO: center text
	 */

	const uint16_t stringWidth = xpcc::GraphicDisplay::getStringWidth(this->label, &(this->font));
	const uint16_t stringHeight = xpcc::GraphicDisplay::getFontHeight(&(this->font));

	if(this->font.isValid())
		out->setFont(&(this->font));

	out->setColor(cp[Color::TEXT]);
	out->setCursor(x + (width - stringWidth) / 2, y + (height - stringHeight) / 2);
	*out << this->label;

}

void xpcc::gui::ArrowButton::render(AbstractView* view)
{
	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

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
	out->drawLine(x, y, x + width - 1, y);
	out->drawLine(x, y, x, y + height - 1);
	out->drawLine(x + width - 1, y + height - 1, x + width - 1, y);
	out->drawLine(x + width, y + height - 1, x, y + height - 1);

}

void xpcc::gui::FilledAreaButton::render(AbstractView* view)
{
	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

	// color palette of view
	ColorPalette *cp = this->color_palette;

	// position and dimensions
	const uint16_t x = this->position.x;
	const uint16_t y = this->position.y;
	const uint16_t width = this->getWidth();
	const uint16_t height = this->getHeight();

	out->setColor(cp[this->color]);
	out->fillRectangle(x, y, width, height);
}
