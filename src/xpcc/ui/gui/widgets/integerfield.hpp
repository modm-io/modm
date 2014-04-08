#ifndef INTEGERFIELD_HPP_
#define INTEGERFIELD_HPP_

#include <xpcc/ui/gui/widgets/widget.hpp>

namespace xpcc {

namespace gui {

class IntegerField : public Widget {

public:

	IntegerField(int16_t default_value, Dimension d) :
		value(default_value)
	{
		this->dimension = d;
	}

	void
	draw(View* view);

	void
	setValue(int16_t value)
	{
		this->value = value;
	}

private:
	int16_t value;
};

}
}

#endif /* INTEGERFIELD_HPP_ */
