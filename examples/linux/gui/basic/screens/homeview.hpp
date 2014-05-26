#ifndef HOMEVIEW_HPP_
#define HOMEVIEW_HPP_

#include <xpcc/ui/gui/abstract_view.hpp>
#include <xpcc/ui/gui/view_stack.hpp>

#include <xpcc/ui/gui.hpp>

class HomeView : public xpcc::gui::AbstractView
{
public:

	HomeView(xpcc::gui::ViewStack* stack, uint8_t identifier);

	~HomeView() {}

	bool
	hasChanged();
private:
	xpcc::gui::ButtonWidget toggleLedButton;
	xpcc::gui::ButtonWidget doNothingButton;
	xpcc::gui::NumberRocker<int16_t> rocker1;
	xpcc::gui::CheckboxWidget checkbox1;

};

#endif /* HOMEVIEW_HPP_ */
