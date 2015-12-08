// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F7DISCOVERY
// Discovery kit for STM32F746 line
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF252419
//

#ifndef XPCC_STM32_F7_DISCOVERY_HPP
#define XPCC_STM32_F7_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

using namespace xpcc::stm32;

namespace Board
{

/// STM32F7 running at 16MHz from the internal 16MHz crystal
// typedef SystemClock< InternalClock<MHz16> > systemClock;
struct systemClock
{
	static constexpr uint32_t Frequency = 16000000;
	static constexpr uint32_t Usart1 = 16000000;

	static void inline enable()
	{
		xpcc::clock::fcpu     = 16000000;
		xpcc::clock::fcpu_kHz = 16000;
		xpcc::clock::fcpu_MHz = 16;
		xpcc::clock::ns_per_loop = 1000/16;
	}
};

typedef GpioInputI11 Button;	// User Button

typedef GpioOutputI1 LedD13;	// User LED 1 (Arduino D13)

namespace stlink
{
typedef GpioA9	Rx;
typedef GpioB7	Tx;
typedef Usart1 Uart;
}

inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Rx::connect(stlink::Uart::Tx);
	stlink::Tx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, 115200>(12);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// XPCC_STM32_F7_DISCOVERY_HPP
