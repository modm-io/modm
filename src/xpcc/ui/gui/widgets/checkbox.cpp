#include "checkbox.hpp"


void xpcc::gui::CheckboxWidget::draw(AbstractView* view)
{
	if(view == NULL)
			return;

	constexpr uint16_t padding = 5;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

	// color palette of view
	ColorPalette *cp = this->color_palette;

	const uint16_t box_width = this->dimension.width;
	const uint16_t box_height = this->dimension.height;
	const uint16_t box_x = this->position.x;
	const uint16_t box_y = this->position.y;

	// draw box
	out->setColor(cp[Color::BORDER]);
	out->drawLine(box_x, box_y, box_x + box_width - 1, box_y);
	out->drawLine(box_x, box_y, box_x, box_y + box_height - 1);
	out->drawLine(box_x + box_width - 1, box_y + box_height - 1, box_x + box_width - 1, box_y);
	out->drawLine(box_x + box_width, box_y + box_height - 1, box_x, box_y + box_height - 1);

	if(state) {

		// draw cross

		out->setColor(cp[Color::TEXT]);
		out->drawLine(box_x + padding, box_y + padding, box_x + box_width - padding, box_y + box_height - padding);
		out->drawLine(box_x + padding, box_y + box_height - padding, box_x + box_width - padding, box_y + padding);


	}
}

void xpcc::gui::CheckboxWidget::click_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) data;

	auto checkbox = static_cast<CheckboxWidget*>(w);

	checkbox->setState(!checkbox->getState());
}
