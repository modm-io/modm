
#include <xpcc/architecture.hpp>

xpcc::stm32::Usart2 uart(115200);

MAIN_FUNCTION
{
	uart.write("Hello World!\n");
	
	while (1)
	{
	}
}

