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
	render(AbstractView* view);

	void
	setValue(T value)
	{
		this->value = value;
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
typedef NumberField<float> FloatField;

}
}

#include "numberfield_impl.hpp"

#endif /* INTEGERFIELD_HPP_ */
