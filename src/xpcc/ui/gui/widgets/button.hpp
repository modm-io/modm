#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include "widget.hpp"

namespace xpcc {

namespace gui {

class ButtonWidget : public Widget {

public:

	ButtonWidget(char* lbl, Dimension d) :
		Widget(d, true),
		label(lbl)
	{
	}

	void
	draw(AbstractView* view);

	void
	setLabel(char* lbl)
	{
		this->label = lbl;
	}

public:
	char *label;

};


class ArrowButton : public Widget {

public:
	ArrowButton(bool orientation, Dimension d) :
		Widget(d, true),
		orientation(orientation)
	{
	}

	void
	draw(AbstractView* view);

private:
	// Orientation of the arrow. false: left | true: right
	bool orientation;


};

}
}

#endif /* BUTTON_HPP_ */
