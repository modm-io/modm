/*
 * Copyright (c) 2015-2018, Niklas Hauser
 * Copyright (c) 2016, Fabian Greif
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2018, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F769I_DISCOVERY_HPP
#define MODM_STM32_F769I_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_disco_f769ni
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_disco_f769ni
namespace Board
{
	using namespace modm::literals;

/// STM32F7 running at 216MHz from the external 25MHz clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 216_MHz;
	static constexpr uint32_t Apb1 = Frequency / 8;
	static constexpr uint32_t Apb2 = Frequency / 4;

	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Adc2 = Apb2;
	static constexpr uint32_t Adc3 = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	static bool inline
	enable()
	{
		Rcc::enableExternalClock(); // 25 MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 25,		// 25MHz / M=25 -> 1MHz
			.pllN = 432,	// 1MHz * N=432 -> 432MHz
			.pllP = 2		// 432MHz / P=2 -> 216MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::ExternalClock, pllFactors);
		// Required for 216 MHz clock
		Rcc::enableOverdriveMode();
		Rcc::setFlashLatency<Frequency>();
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		// APB1 is running only at 27MHz, since AHB / 4 = 54MHz > 45MHz limit!
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div8);
		// APB2 is running only at 54MHz, since AHB / 2 = 108MHz > 90MHz limit!
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div4);
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};


using Button = GpioInputA0;	// User Button
using LedJ13 = GpioOutputJ13;	// User LED 1 (red)
using LedJ5  = GpioOutputJ5;	// User LED 2 (green)
using LedA12 = GpioOutputA12;	// User LED 3 (green)
using LedD4  = GpioInverted<GpioOutputD4>;	// User Led 4 (red)

using Leds = SoftwareGpioPort< LedJ13, LedJ5, LedA12, LedD4 >;


namespace stlink
{
using Tx = GpioOutputA9;
using Rx = GpioInputA10;
using Uart = Usart1;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

// Onboard MicroSD card slot
namespace usd
{

// SDIO interface
namespace sdio
{
using Cmd = GpioOutputD7;
using Clk = GpioOutputD7;
using D3  = GpioB4;
using D2  = GpioB3;
using D1  = GpioG10;
using D0  = GpioG9;
}

// SPI interface
namespace spi
{
using Mosi = sdio::Cmd;
using Clk  = sdio::Clk;
using Cs   = sdio::D3;
using Irq  = sdio::D1;
using Miso = sdio::D0;
}
}

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// MODM_STM32_F769I_DISCOVERY_HPP
