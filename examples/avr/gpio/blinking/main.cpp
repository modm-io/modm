
#include <xpcc/architecture.hpp>

typedef GpioOutputB0 Led;

int
main(void)
{
	Led::setOutput();
	Led::set();

	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(500);
	}
}
