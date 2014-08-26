#ifndef OVERVIEW_HPP_
#define OVERVIEW_HPP_


#include <xpcc/ui/gui/view.hpp>
#include <xpcc/ui/gui/view_stack.hpp>

#include <xpcc/ui/gui.hpp>

using namespace xpcc::gui;

class Overview : public xpcc::gui::View
{
public:

	Overview(xpcc::gui::GuiViewStack* stack, uint8_t identifier);

	~Overview() {}

	bool
	hasChanged();

public:
	xpcc::gui::TabPanel tabpanel;
	xpcc::gui::ButtonWidget buttonLeft;
	xpcc::gui::ButtonWidget buttonMiddle;
	xpcc::gui::ButtonWidget buttonRight;

};

#endif /* OVERVIEW_HPP_ */
