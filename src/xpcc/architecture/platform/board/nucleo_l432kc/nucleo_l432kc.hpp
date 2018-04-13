// coding: utf-8
/* Copyright (c) 2018, Christopher Durand
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

//
// NUCLEO-L432KC
// Nucleo kit for STM32L432KC
// http://www.st.com/en/evaluation-tools/nucleo-l432kc.html
//

#ifndef XPCC_STM32_NUCLEO_L432KC_HPP
#define XPCC_STM32_NUCLEO_L432KC_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#define XPCC_BOARD_HAS_LOGGER

using namespace xpcc::stm32;

namespace Board
{

/// STM32L4 running at 80MHz generated with the PLL from 4MHz MSI clock

struct systemClock {
	static constexpr uint32_t Frequency = 80 * MHz1;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usart4 = Apb1;
	static constexpr uint32_t Usart5 = Apb1;

	static bool inline
	enable()
	{
		ClockControl::enablePll(
			ClockControl::PllSource::MultiSpeedInternalClock,
			1,	// 4MHz / M=1 -> 4MHz
			40,	// 4MHz * N=40 -> 160MHz <= 344MHz = PLL VCO output max, >= 64 MHz = PLL VCO out min
			2,	// 160MHz / P=2 -> 80MHz = F_cpu
			2	// 80MHz / Q=2 -> 40MHz = F_usb
		);
		ClockControl::setFlashLatency(Frequency);

		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		// APB1 has max. 80MHz
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div1);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

		return true;
	}
};

using LedGreen = GpioOutputB3;

namespace stlink
{
	using Tx = GpioOutputA2;
	using Rx = GpioInputA15;
	using Uart = Usart2;
}

using LoggerDevice = xpcc::IODeviceWrapper< stlink::Uart, xpcc::IOBuffer::BlockIfFull >;


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Tx::connect(stlink::Uart::Tx);
	stlink::Rx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, xpcc::Uart::Baudrate::B115200>(12);

	LedGreen::setOutput(xpcc::Gpio::Low);
}

}

#endif	// XPCC_STM32_NUCLEO_L432KC_HPP
