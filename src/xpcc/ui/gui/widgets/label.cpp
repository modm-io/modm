#include "label.hpp"
#include <xpcc/ui/gui/colorpalette.hpp>

void xpcc::gui::Label::render(AbstractView* view)
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
