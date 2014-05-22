#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <xpcc/ui/display/font.hpp>
#include "widget.hpp"

namespace xpcc {

namespace gui {

class ButtonWidget : public Widget {

public:

	ButtonWidget(const char* lbl, Dimension d) :
		Widget(d, true),
		label(lbl)
	{
	}

	void
	render(AbstractView* view);

	void
	setLabel(char* lbl)
	{
		this->label = lbl;
	}

private:
	const char *label;
};


class ArrowButton : public Widget {

public:
	ArrowButton(bool orientation, Dimension d) :
		Widget(d, true),
		orientation(orientation)
	{
	}

	void
	render(AbstractView* view);

private:
	// Orientation of the arrow. false: left | true: right
	bool orientation;


};


class FilledAreaButton : public Widget {

public:
	FilledAreaButton(xpcc::glcd::Color color, Dimension d) :
		Widget(d, true),
		color(color)
	{
	}

	void
	setBackgroundColor(xpcc::glcd::Color color)
	{
		this->color = color;
	}

	void
	render(AbstractView* view);

private:
	xpcc::glcd::Color color;

};

}
}

#endif /* BUTTON_HPP_ */
