
#include <xpcc/architecture.hpp>

typedef GpioOutputB0 Led;
typedef GpioOutputB1 Led2;

typedef xpcc::GpioInverted< Led2 > LedInverted;

int
main(void)
{
	Led::setOutput();
	Led::set();

	LedInverted::setOutput();
	LedInverted::reset();

	while (1)
	{
	}
}
