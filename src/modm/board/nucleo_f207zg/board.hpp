/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2016-2018, Niklas Hauser
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
#include <modm/debug/logger.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nucleo_f207zg
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_f207zg
/// @{
using namespace modm::literals;

/// STM32F207ZG running at 120MHz from the ST-LINK 8 MHz MCO
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 4,   //   8 MHz /   4 =   2 MHz
		.N = 240, //   2 MHz * 240 = 480 MHz
		.P = 4,   // 480 MHz /   4 = 120 MHz = F_cpu
		.Q = 10,  // 480 MHz /  10 =  48 MHz = F_usb
	};
	static constexpr uint32_t PllP = Hse / pll.M * pll.N / pll.P;
	static constexpr uint32_t PllQ = Hse / pll.M * pll.N / pll.Q;

	static constexpr uint32_t Frequency = PllP;
	static_assert(Frequency == Rcc::MaxFrequency);

	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Flash = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Eth = Ahb1;

	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Rng = Ahb2;

	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	// APB2 domain
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Sdio = Apb2;
	static constexpr uint32_t Syscfg = Apb2;

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

	static constexpr uint32_t Usb = PllQ;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = 32.768_kHz;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseClock();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);

		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

// Arduino Footprint
#include "nucleo144_arduino.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputB14;	// LED3 [Red]
using Leds = SoftwareGpioPort< LedRed, LedBlue, LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_f207zg
/// @{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputG7;	// OTG_FS_OverCurrent
using Power = GpioOutputG6;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_f207zg
/// @{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = BufferedUart<UsartHal3, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_f207zg
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Leds::setOutput(modm::Gpio::Low);

	Button::setInput();
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// // Enable VBUS sense (B device) via pin PA9
	// USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
	// USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
}
/// @}

}
