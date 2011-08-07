
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LED, B, 0);

int
main(void)
{
	LED::setOutput();
	LED::set();
	
	while (1)
	{
		LED::toggle();
		xpcc::delay_ms(500);
	}
}
