#include "homeview.hpp"

HomeView::HomeView(xpcc::gui::ViewStack* stack, uint8_t identifier) :
	AbstractView(stack, identifier, xpcc::gui::Dimension(320, 240))
{
}

bool
HomeView::hasChanged()
{
	return true;
}
