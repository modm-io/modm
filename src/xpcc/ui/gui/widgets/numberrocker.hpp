#ifndef INTEGEROCKER_HPP_
#define INTEGEROCKER_HPP_

#include "widget.hpp"
#include "button.hpp"
#include "integerfield.hpp"

namespace xpcc {

namespace gui {

class NumberRocker : public WidgetGroup {

public:

	NumberRocker(int16_t default_value, int16_t step, Dimension d) :
		WidgetGroup(d),
		value(default_value),
		step(step),
		button_increase(true, Dimension(d.height, d.height)),
		button_decrease(false, Dimension(d.height, d.height)),
		int_field(default_value, Dimension(d.width - 2*d.height, d.height))
	{
		this->dimension = d;

		button_decrease.cb_activate = &decrease_cb;
		button_increase.cb_activate = &increase_cb;

		this->pack(&button_decrease, xpcc::glcd::Point(0,0));
		this->pack(&button_increase, xpcc::glcd::Point(d.width - d.height,0));
		this->pack(&int_field, xpcc::glcd::Point(d.height, 0));

	}

	void
	increase();

	void
	decrease();

	void
	activate(const InputEvent& ev, void* data);

	void
	deactivate(const InputEvent& ev, void* data);

	void
	handleInputEvent(const InputEvent& ev);

private:
	static void
	increase_cb(const InputEvent& ev, Widget* w, void* data);

	static void
	decrease_cb(const InputEvent& ev, Widget* w, void* data);

private:
	int16_t value;
	int16_t step;
	ArrowButton button_increase, button_decrease;
	IntegerField int_field;

};

}
}

#endif /* INTEGEROCKER_HPP_ */
