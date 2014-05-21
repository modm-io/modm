#include "homeview.hpp"

HomeView::HomeView(xpcc::gui::ViewStack* stack, uint8_t identifier) :
	AbstractView(stack, identifier, xpcc::gui::Dimension(320, 240)),
	toggleLedButton((char*)"Win Eurobot", xpcc::gui::Dimension(100, 50)),
	doNothingButton((char*)"Delete XPCC", xpcc::gui::Dimension(100, 50)),
	rocker1(100, 50, xpcc::gui::Dimension(200, 30)),
	checkbox1(true, xpcc::gui::Dimension(30, 30))
{
	this->pack(&toggleLedButton, xpcc::glcd::Point(110, 10));
	this->pack(&doNothingButton, xpcc::glcd::Point(110, 80));
	this->pack(&checkbox1, xpcc::glcd::Point(60, 140));
	this->pack(&rocker1, xpcc::glcd::Point(60, 200));
}

bool
HomeView::hasChanged()
{
	return true;
}
