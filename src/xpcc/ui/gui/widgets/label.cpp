// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "label.hpp"
#include <xpcc/ui/gui/colorpalette.hpp>

void xpcc::gui::Label::render(View* view)
{
	if(view == NULL)
		return;

	// output device of view
	xpcc::GraphicDisplay* out = &view->display();

	out->setColor(color);

	if(this->font.isValid())
	{
		out->setFont(&(this->font));
	}

	out->setCursor(this->getPosition().x, this->getPosition().y);
	*out << this->label;
}
