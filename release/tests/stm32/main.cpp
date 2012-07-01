
#include <xpcc/architecture.hpp>

xpcc::stm32::Usart1             uart1(115200);
xpcc::stm32::BufferedUsart2     uart2(115200, 15);
xpcc::stm32::BufferedFlowUsart3 uart3(115200, 15);

#if defined(STM32F10X_HD) || \
	defined(STM32F10X_XL) || \
	defined(STM32F10X_CL) || \
	defined(STM32F2XX)    || \
	defined(STM32F4XX)

xpcc::stm32::Uart4          uart4(115200);
xpcc::stm32::BufferedUart5  uart5(115200, 15);
#endif

#if defined(STM32F2XX) || defined(STM32F4XX)
xpcc::stm32::BufferedFlowUsart6 uart6(115200, 15);
#endif

GPIO__OUTPUT(Out, A, 0);
GPIO__INPUT(In, A, 1);
GPIO__IO(Io, A, 2);

int
main(void)
{
	Out::setOutput();
	Out::setOutput(true);
	Out::set();
	Out::reset();	
	Out::toggle();
	
	In::setInput();
	In::read();
	
	Io::setOutput();
	Io::set();
	Io::setInput();
	Io::read();
	
	uart1.write('x');
	uart2.write('x');
	uart3.write('x');

#if defined(STM32F10X_HD) || \
	defined(STM32F10X_XL) || \
	defined(STM32F10X_CL) || \
	defined(STM32F2XX)    || \
	defined(STM32F4XX)

	uart4.write('x');
	uart5.write('x');
#endif

#if defined(STM32F2XX) || defined(STM32F4XX)
	uart6.write('x');
#endif

	while (1)
	{
	}
}
