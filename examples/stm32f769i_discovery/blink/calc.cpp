#include "calc.hpp"

double Calc::myDouble = double(1.0);

void
Calc::calc()
{
	myDouble += double(0.1);
}

double
Calc::get()
{
	return myDouble;
}
