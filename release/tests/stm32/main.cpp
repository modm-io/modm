/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture.hpp>

#if !defined(STM32F3XX)
// Not yet implemented for STM32 F3 series
//xpcc::stm32::Usart1             uart1(115200);
//xpcc::stm32::BufferedUsart2     uart2(115200, 15);
//xpcc::stm32::BufferedFlowUsart3 uart3(115200, 15);
#endif

#if defined(STM32F10X_HD) || \
	defined(STM32F10X_XL) || \
	defined(STM32F10X_CL) || \
	defined(STM32F2XX)    || \
	defined(STM32F4XX)

//xpcc::stm32::Uart4          uart4(115200);
//xpcc::stm32::BufferedUart5  uart5(115200, 15);
#endif

#if defined(STM32F2XX) || defined(STM32F4XX)
//xpcc::stm32::BufferedFlowUsart6 uart6(115200, 15);
#endif

typedef xpcc::stm32::GpioOutputA0 Out;
typedef xpcc::stm32::GpioInputA1  In;
typedef xpcc::stm32::GpioA2       Io;

int
main()
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
	
#if !defined(STM32F3XX)
//	uart1.write('x');
//	uart2.write('x');
//	uart3.write('x');
#endif

#if defined(STM32F10X_HD) || \
	defined(STM32F10X_XL) || \
	defined(STM32F10X_CL) || \
	defined(STM32F2XX)    || \
	defined(STM32F4XX)

//	uart4.write('x');
//	uart5.write('x');
#endif

#if defined(STM32F2XX) || defined(STM32F4XX)
//	uart6.write('x');
#endif

	while (1)
	{
	}
}
