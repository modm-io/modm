#ifndef INTEGERFIELD_HPP_
#define INTEGERFIELD_HPP_

#include "widget.hpp"

namespace xpcc {

namespace gui {

template<typename T>
class NumberField : public Widget {

public:

	NumberField(T default_value, Dimension d) :
		Widget(d, false),
		value(default_value)
	{
	}

	void
	render(View* view);

	void
	setValue(T value)
	{
		if(this->value == value)
			return;
		this->value = value;
		this->markDirty();
	}

	T
	getValue()
	{
		return this->value;
	}

private:
	T value;
};

typedef NumberField<int16_t> IntegerField;


class FloatField : public NumberField<float>
{
public:
	FloatField(float value, Dimension d);

	void
	render(View* view);
};


}
}

#include "numberfield_impl.hpp"

#endif /* INTEGERFIELD_HPP_ */
