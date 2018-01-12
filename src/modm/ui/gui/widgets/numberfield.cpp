/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2017, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <math.h>

#include "numberfield.hpp"

modm::gui::FloatField::FloatField(float value, Dimension d) :
	NumberField<float>(value, d)
{
}

void
modm::gui::FloatField::render(View* view)
{
	if(view == 0)
		return;

	// output device of view
	modm::GraphicDisplay* out = &view->display();

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

	int beforeComma = static_cast<int>(this->getValue());
	int afterComma = std::abs(static_cast<int>((this->getValue() - beforeComma) * 1000));

	*out << beforeComma << ".";
	out->printf("%03d", afterComma);

//	*out << this->value;
}
