/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F4XX_DEVEBOX_HPP
#define MODM_STM32_F4XX_DEVEBOX_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/storage/block_device_spiflash.hpp>

using namespace modm::platform;

/// @ingroup modm_board_devebox_f4xx
namespace Board
{
	using namespace modm::literals;

/// STM32F407 running at 168MHz generated from the external 8MHz crystal
struct SystemClock {
	static constexpr uint32_t Frequency = 168_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Can1   = Apb1;
	static constexpr uint32_t Can2   = Apb1;

	static constexpr uint32_t Spi1   = Apb2;
	static constexpr uint32_t Spi2   = Apb1;
	static constexpr uint32_t Spi3   = Apb1;
	static constexpr uint32_t Spi4   = Apb2;
	static constexpr uint32_t Spi5   = Apb2;
	static constexpr uint32_t Spi6   = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;
	static constexpr uint32_t I2c3   = Apb1;

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
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();	// 8MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 4,		// 8MHz / M=4 -> 2MHz
			.pllN = 168,	// 2MHz * N=168 -> 336MHz
			.pllP = 2		// 336MHz / P=2 -> 168MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);
		// set flash latency for 168MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 42MHz
		// APB2 has max. 84MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};


using Button = GpioInputA0;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using LedGreen = GpioInverted<GpioOutputA1>;	// User LED

using Leds = SoftwareGpioPort< LedGreen >;

namespace usb
{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
}

namespace w25q16
{
using Cs   = GpioOutputA15;
using Sck  = GpioOutputB3;
using Mosi = GpioOutputB5;
using Miso = GpioInputB4;

using SpiMaster = SpiMaster1;

constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 2*1024*1024; // 16 MiBits
using StorageDevice = modm::BdSpiFlash<SpiMaster, Cs, MemorySize>;
}


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);

	Button::setInput(Gpio::InputType::PullDown);
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}


inline void
initializeW25q16()
{
	w25q16::Cs::setOutput(modm::Gpio::High);

	w25q16::SpiMaster::connect<w25q16::Sck::Sck, w25q16::Mosi::Mosi, w25q16::Miso::Miso>();
	w25q16::SpiMaster::initialize<SystemClock, 42_MHz>();
}

}

#endif	// MODM_STM32_F4XX_DEVEBOX_HPP
