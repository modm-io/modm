#ifndef BUTTON_HPP_
#define BUTTON_HPP_

#include <xpcc/ui/gui/widgets/widget.hpp>

namespace xpcc {

namespace gui {

class ButtonWidget : public Widget {

public:

	ButtonWidget(char* lbl, Dimension d) :
		label(lbl)
	{
		this->dimension = d;
	}

	ButtonWidget(char* lbl, uint16_t width, uint16_t height) :
		label(lbl)
	{
		this->dimension.width = width;
		this->dimension.height = height;
	}

	void
	draw(View* view);

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
		orientation(orientation)
	{
		this->dimension = d;
	}

	void
	draw(View* view);

private:
	// Orientation of the arrow. false: left | true: right
	bool orientation;


};

}
}

#endif /* BUTTON_HPP_ */
