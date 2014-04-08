#ifndef INTEGERFIELD_HPP_
#define INTEGERFIELD_HPP_

#include <xpcc/ui/gui/widgets/widget.hpp>

namespace xpcc {

namespace gui {

template<typename T>
class NumberField : public Widget {

public:

	NumberField(T default_value, Dimension d) :
		value(default_value)
	{
		this->dimension = d;
	}

	void
	draw(View* view);

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
