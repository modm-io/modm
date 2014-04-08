#include "numberrocker.hpp"

void xpcc::gui::NumberRocker::increase()
{
	this->value += this->step;
	this->int_field.setValue(this->value);
}

void xpcc::gui::NumberRocker::decrease()
{
	this->value -= this->step;
	this->int_field.setValue(this->value);
}

void xpcc::gui::NumberRocker::activate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(ev);
}

void xpcc::gui::NumberRocker::deactivate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(ev);
}

void xpcc::gui::NumberRocker::increase_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) w;

	NumberRocker *rocker = static_cast<NumberRocker*>(data);

	rocker->increase();
}

void xpcc::gui::NumberRocker::decrease_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) w;

	NumberRocker *rocker = static_cast<NumberRocker*>(data);

	rocker->decrease();
}

void xpcc::gui::NumberRocker::handleInputEvent(const InputEvent& ev)
{
	// convert to relative coordinate
	uint16_t ev_x = ev.coord.x - this->position.x;

	Widget* w = 0;


	if(ev_x < this->dimension.height) {
		w = &button_decrease;
	} else if(ev_x > (this->dimension.width - this->dimension.height)) {
		w = &button_increase;
	} else {
		// text field, do nothing
	}
	if(!w)
		return;

	if(ev.direction == xpcc::gui::InputEvent::Direction::UP) {
		w->deactivate(ev, this);
	} else if(ev.direction == xpcc::gui::InputEvent::Direction::DOWN) {
		w->activate(ev, this);
	}
}
