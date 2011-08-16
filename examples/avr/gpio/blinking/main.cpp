
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Led, B, 0);

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
