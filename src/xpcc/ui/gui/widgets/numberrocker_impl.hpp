#ifndef INTEGEROCKER_HPP_
	#error	"Don't include this file directly, use 'button_group.hpp' instead!"
#endif

#include "numberrocker.hpp"

template<typename T>
void xpcc::gui::NumberRocker<T>::increase()
{
	this->value += this->step;
	this->num_field.setValue(this->value);
}

template<typename T>
void xpcc::gui::NumberRocker<T>::decrease()
{
	this->value -= this->step;
	this->num_field.setValue(this->value);
}

template<typename T>
void xpcc::gui::NumberRocker<T>::activate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(ev);
}

template<typename T>
void xpcc::gui::NumberRocker<T>::deactivate(const InputEvent& ev, void* data)
{
	(void) data;

	this->handleInputEvent(ev);
}

template<typename T>
void xpcc::gui::NumberRocker<T>::increase_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) w;

	NumberRocker *rocker = static_cast<NumberRocker*>(data);

	rocker->increase();
}

template<typename T>
void xpcc::gui::NumberRocker<T>::decrease_cb(const InputEvent& ev, Widget* w, void* data)
{
	(void) ev;
	(void) w;

	NumberRocker *rocker = static_cast<NumberRocker*>(data);

	rocker->decrease();
}

template<typename T>
void xpcc::gui::NumberRocker<T>::handleInputEvent(const InputEvent& ev)
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
