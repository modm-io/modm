#ifndef LABEL_HPP_
#define LABEL_HPP_

#include "widget.hpp"

namespace xpcc {

namespace gui {

class Label : public Widget {

public:
	Label(const char* lbl, xpcc::glcd::Color color) :
		Widget(Dimension(0,0), false),
		label(lbl),
		color(color)
	{
		this->updateDimension();
	}

	void
	render(AbstractView* view);

	void
	setColor(xpcc::glcd::Color color)
	{
		this->color = color;
	}

	void
	setLabel(char* lbl)
	{
		this->label = lbl;
		this->updateDimension();
	}

	void
	setFont(const xpcc::accessor::Flash<uint8_t> *font)
	{
		this->font = *font;
		this->updateDimension();
	}

	void
	setFont(const uint8_t *newFont)
	{
		this->font = xpcc::accessor::asFlash(newFont);
		this->updateDimension();
	}

private:

	void
	updateDimension()
	{
		/* Update label dimension */
		if(this->font.isValid()) {
			this->dimension.width = xpcc::GraphicDisplay::getStringWidth(this->label, &(this->font));
			this->dimension.height = xpcc::GraphicDisplay::getFontHeight(&(this->font));
		}
	}

private:
	const char* label;
	xpcc::glcd::Color color;

};

//
//class InteractiveLabel : public Label {
//
//public:
//	InteractiveLabel(const char* lbl) :
//		Label(lbl)
//	{
//		this->is_interactive = true;
//	}
//};

}
}

#endif /* LABEL_HPP_ */
