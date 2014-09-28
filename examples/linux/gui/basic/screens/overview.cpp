#include "overview.hpp"

#include <xpcc/ui/gui/widgets.hpp>

using namespace xpcc::gui;

Overview::Overview(xpcc::gui::GuiViewStack* stack, uint8_t identifier) :
	View(stack, identifier, Dimension(320, 240)),
	tabpanel(Dimension(320, 240), 0.2),
	buttonLeft("Back", tabpanel.buttonDimension),
	buttonMiddle("change", tabpanel.buttonDimension),
	buttonRight("Repos.", tabpanel.buttonDimension)
{

	tabpanel.packTabLeft(&buttonLeft);
	tabpanel.packTabMiddle(&buttonMiddle);
	tabpanel.packTabRight(&buttonRight);


	this->pack(&tabpanel, xpcc::glcd::Point(0, 0));

}

bool Overview::hasChanged()
{
	return true;
}
