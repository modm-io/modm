
#include <xpcc/architecture/general/gpio.hpp>
#include <xpcc/architecture/general/time.hpp>

GPIO__OUTPUT(LED, B, 0);

int
main(void)
{
	LED::setOutput();
	LED::set();
	
	while (1)
	{
		LED::toggle();
		xpcc::delay_ms(1.5);
	}
}
