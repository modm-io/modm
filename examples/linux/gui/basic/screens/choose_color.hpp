#ifndef CHOOSE_COLOR_HPP_
#define CHOOSE_COLOR_HPP_


#include <xpcc/ui/gui/abstract_view.hpp>
#include <xpcc/ui/gui/view_stack.hpp>

#include <xpcc/ui/gui.hpp>

using namespace xpcc::gui;

class ChooseColorView : public xpcc::gui::AbstractView
{
public:

	ChooseColorView(xpcc::gui::ViewStack* stack, uint8_t identifier);

	~ChooseColorView() {}

	bool
	hasChanged();

	static void
	activate_yellow(const InputEvent&, Widget*, void*);

	static void
	deactivate_yellow(const InputEvent&, Widget*, void*);

	static void
	activate_red(const InputEvent&, Widget*, void*);

	static void
	deactivate_red(const InputEvent&, Widget*, void*);

public:
//	xpcc::gui::ButtonWidget button1;
	xpcc::gui::Label lbl_yellow;
	xpcc::gui::Label lbl_red;
	xpcc::gui::FilledAreaButton yellow;
	xpcc::gui::FilledAreaButton red;


};

#endif /* CHOOSE_COLOR_HPP_ */
