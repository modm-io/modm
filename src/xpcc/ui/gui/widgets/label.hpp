#ifndef LABEL_HPP_
#define LABEL_HPP_

#include "widget.hpp"

namespace xpcc {

namespace gui {

class Label : public Widget {

public:
	Label(char* lbl) :
		Widget(Dimension(0,0), false),
		label(lbl)
	{
		this->updateDimension();
	}

	void
	draw(AbstractView* view);

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
	char* label;

};

class InteractiveLabel : public Label {

public:
	InteractiveLabel(char* lbl, Dimension d) :
		Label(lbl)
	{
		this->is_interactive = true;
	}
};

}
}

#endif /* LABEL_HPP_ */
