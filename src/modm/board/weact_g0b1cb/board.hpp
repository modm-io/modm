/*
 * Copyright (c) 2025, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_weact_g0b1cb
/// @{
using namespace modm::literals;

/// STM32G0B1 running at 64MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr Rcc::PllConfig pll{
		.M = 2,  //   8 MHz /  2 =   4 MHz
		.N = 48, //   4 MHz * 48 = 192 MHz
		.Q = 4,  // 192 MHz /  4 =  48 MHz = F_usb
		.R = 3,  // 192 MHz /  3 =  64 MHz = F_cpu
	};
	static constexpr uint32_t PllR = Hse / pll.M * pll.N / pll.R;
	static constexpr uint32_t PllQ = Hse / pll.M * pll.N / pll.Q;
	static_assert(PllR == Rcc::MaxFrequency);

	static constexpr uint32_t SysClk = PllR;
	static constexpr uint32_t Frequency = SysClk;

	static constexpr uint32_t Ahb = SysClk;
	static constexpr uint32_t Apb = SysClk / 1;
	static constexpr uint32_t ApbTimer = Apb * 1;

	static constexpr uint32_t Aes  = Ahb;
	static constexpr uint32_t Rng  = Ahb;
	static constexpr uint32_t Crc  = Ahb;
	static constexpr uint32_t Flash  = Ahb;
	static constexpr uint32_t Exti = Ahb;
	static constexpr uint32_t DmaMux = Ahb;
	static constexpr uint32_t Dma2 = Ahb;
	static constexpr uint32_t Dma1 = Ahb;

	static constexpr uint32_t Dbg = Apb;
	static constexpr uint32_t Timer17 = ApbTimer;
	static constexpr uint32_t Timer16 = ApbTimer;
	static constexpr uint32_t Timer15 = ApbTimer;
	static constexpr uint32_t Usart6 = Apb;
	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Spi1 = Apb;
	static constexpr uint32_t I2s1 = Apb;
	static constexpr uint32_t Timer1 = ApbTimer;
	static constexpr uint32_t Adc = Apb;
	static constexpr uint32_t Comp = Apb;
	static constexpr uint32_t Syscfg = Apb;
	static constexpr uint32_t Itline = Apb;
	static constexpr uint32_t Vrefbuf = Apb;
	static constexpr uint32_t Fdcan = Apb;
	static constexpr uint32_t Tamp = Apb;
	static constexpr uint32_t Bkp = Apb;
	static constexpr uint32_t Ucpd2 = Apb;
	static constexpr uint32_t Ucpd1 = Apb;
	static constexpr uint32_t Lptim2 = Apb;
	static constexpr uint32_t I2c3 = Apb;
	static constexpr uint32_t Lpuart2 = Apb;
	static constexpr uint32_t Lpuart1 = Apb;
	static constexpr uint32_t Lptim1 = Apb;
	static constexpr uint32_t Cec = Apb;
	static constexpr uint32_t Dac = Apb;
	static constexpr uint32_t Pwr = Apb;
	static constexpr uint32_t Crs = Apb;
	static constexpr uint32_t Fdcan2 = Apb;
	static constexpr uint32_t Fdcan1 = Apb;

	static constexpr uint32_t I2c2 = Apb;
	static constexpr uint32_t I2c1 = Apb;
	static constexpr uint32_t Usart5 = Apb;
	static constexpr uint32_t Usart4 = Apb;
	static constexpr uint32_t Usart3 = Apb;
	static constexpr uint32_t Usart2 = Apb;
	static constexpr uint32_t Spi3 = Apb;
	static constexpr uint32_t Spi2 = Apb;
	static constexpr uint32_t I2s2 = Apb;
	static constexpr uint32_t Wwdg = Apb;
	static constexpr uint32_t Timer14 = ApbTimer;
	static constexpr uint32_t Timer7 = ApbTimer;
	static constexpr uint32_t Timer6 = ApbTimer;
	static constexpr uint32_t Timer4 = ApbTimer;
	static constexpr uint32_t Timer3 = ApbTimer;
	static constexpr uint32_t Timer2 = ApbTimer;

	static constexpr uint32_t Usb = PllQ;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = 32.768_kHz;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();
		Rcc::setFlashLatency<Frequency>();

		Rcc::enablePll(Rcc::PllSource::Hse, pll);

		Rcc::enableSystemClock(Rcc::SystemClockSource::PllR);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::PllQ);

		return true;
	}
};
/// @}

namespace usb
{
/// @ingroup modm_board_weact_g0b1cb
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_weact_g0b1cb
/// @{
using Led = GpioOutputC6;
using Leds = SoftwareGpioPort< Led >;

using Button = GpioInputC13;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Led::setOutput(modm::Gpio::Low);
	Button::setInput(Gpio::InputType::PullDown);
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

} // Board namespace
