/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "checkbox.hpp"


void
modm::gui::CheckboxWidget::render(View* view)
{
	if(view == NULL)
		return;

	constexpr uint16_t padding = 5;

	// output device of view
	modm::ColorGraphicDisplay* out = &view->display();

	// color palette of view
	ColorPalette cp = this->color_palette;

	const uint16_t box_width = this->dimension.width;
	const uint16_t box_height = this->dimension.height;
	const uint16_t box_x = this->getPosition().x();
	const uint16_t box_y = this->getPosition().y();

	// clear background
	out->setColor(cp[Color::BACKGROUND]);
	out->fillRectangle(this->getPosition(), this->getWidth(), this->getHeight());

	// draw box
	out->setColor(cp[Color::BORDER]);
	out->drawLine(box_x, box_y, box_x + box_width - 1, box_y);
	out->drawLine(box_x, box_y, box_x, box_y + box_height - 1);
	out->drawLine(box_x + box_width - 1, box_y + box_height - 1, box_x + box_width - 1, box_y);
	out->drawLine(box_x + box_width, box_y + box_height - 1, box_x, box_y + box_height - 1);

	if(state)
	{
		// draw cross
		out->setColor(cp[Color::TEXT]);
		out->drawLine(box_x + padding, box_y + padding, box_x + box_width - padding, box_y + box_height - padding);
		out->drawLine(box_x + padding, box_y + box_height - padding, box_x + box_width - padding, box_y + padding);
	}
}

void
modm::gui::CheckboxWidget::click_cb(const InputEvent& /*ev*/, Widget* w, void* /*data*/)
{
	auto checkbox = static_cast<CheckboxWidget*>(w);

	checkbox->setState(!checkbox->getState());
}
