#ifndef STRINGFIELD_HPP_
#define STRINGFIELD_HPP_

#include "widget.hpp"
#include "label.hpp"

namespace xpcc {

namespace gui {

class StringField : public Widget {

public:

	StringField(const char* value, Dimension d) :
		Widget(d, false),
		value(value)
	{
	}

	void
	render(View* view);

	void
	setValue(const char* value)
	{
		if(this->value == value)
			return;
		this->value = value;
		this->markDirty();
	}

	const char*
	getValue()
	{
		return this->value;
	}

private:
	const char* value;
};

}
}

#endif /* STRINGFIELD_HPP_ */
