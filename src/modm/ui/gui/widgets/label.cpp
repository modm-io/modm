/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "label.hpp"
#include <modm/ui/gui/colorpalette.hpp>

void
modm::gui::Label::render(View* view)
{
	if(view == NULL)
		return;

	// output device of view
	modm::GraphicDisplay* out = &view->display();

	out->setColor(color);

	if(this->font.isValid())
	{
		out->setFont(&(this->font));
	}

	out->setCursor(this->getPosition().x, this->getPosition().y);
	*out << this->label;
}
