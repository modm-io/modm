
#include <xpcc/architecture.hpp>

using namespace xpcc::atmega;

typedef GpioOutputB0 Led;
typedef GpioOutputB1 Led2;
typedef GpioPortD<4, 4> Data;

typedef xpcc::GpioInverted< Led2 > LedInverted;

int
main(void)
{
	Led::setOutput();
	Led::set();
	Data::setOutput();
	Data::write(0x02);

	LedInverted::setOutput();
	LedInverted::reset();

	while (1)
	{
	}
}
