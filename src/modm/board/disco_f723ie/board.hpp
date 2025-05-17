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

#pragma once

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_disco_f723ie
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_disco_f723ie
/// @{
using namespace modm::literals;

/// STM32F7 running at 216MHz from the external 25MHz clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 216_MHz;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

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

	static constexpr uint32_t Usb = 48_MHz;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = 32.768_kHz;

	static bool inline
	enable()
	{
		Rcc::enableLowSpeedExternalCrystal();
		Rcc::enableRealTimeClock(Rcc::RealTimeClockSource::LowSpeedExternalCrystal);

		Rcc::enableExternalClock(); // 25 MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 25,		// 25MHz / M=25 -> 1MHz
			.pllN = 432,	// 1MHz * N=432 -> 432MHz
			.pllP = 2,		// 432MHz / P=2 -> 216MHz = F_cpu
			.pllQ = 9		// 432MHz / Q=9 -> 48MHz = F_usb
		};
		Rcc::enablePll(Rcc::PllSource::ExternalClock, pllFactors);
		// Required for 216 MHz clock
		Rcc::enableOverdriveMode();

		Rcc::setFlashLatency<Frequency>();
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		// APB1 is running at 54MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div4);
		// APB2 is running at 108MHz
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino footprint
using A0 = GpioA6;
using A1 = GpioA4;
using A2 = GpioC4;
using A3 = GpioF10;
using A4 = GpioC0;
using A5 = GpioC1;

using D0  = GpioA3;
using D1  = GpioA2;
using D2  = GpioC5;
using D3  = GpioE5;
using D4  = GpioH3;
using D5  = GpioB0;
using D6  = GpioE6;
using D7  = GpioE3;
using D8  = GpioE4;
using D9  = GpioH6;
using D10 = GpioA1;
using D11 = GpioB5;
using D12 = GpioB4;
using D13 = GpioOutputA5;
using D14 = GpioH5;
using D15 = GpioH4;

using Button = GpioInputA0;		// User Button
using LedRed = GpioOutputA7;	// User Red LED
using LedGreen = GpioOutputB1;	// User Green LED
using LedD13 = D13;				// User Blue LED (Arduino D13)

using Leds = SoftwareGpioPort< LedD13, LedGreen, LedRed >;
/// @}

namespace usb_fs
{
/// @ingroup modm_board_disco_f723ie
/// @{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;
using Overcurrent = GpioB10;
using Power = GpioG8;

using Device = UsbFs;
/// @}
}

namespace usb_hs
{
/// @ingroup modm_board_disco_f723ie
/// @{

using Vbus = GpioB13;
using Id = GpioB12;
using Dm = GpioB14;
using Dp = GpioB15;
using Overcurrent = GpioH10;
using Power = GpioH12;

using Device = UsbHs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_disco_f723ie
/// @{
using Tx = GpioOutputC6;
using Rx = GpioInputC7;
using Uart = BufferedUart<UsartHal6, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_disco_f723ie
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput();
}

inline void
initializeUsb(uint8_t priority=3)
{
	// Full-speed USB port
	usb_fs::Device::initialize<SystemClock>(priority);
	usb_fs::Device::connect<usb_fs::Dm::Dm, usb_fs::Dp::Dp, usb_fs::Id::Id>();
	usb_fs::Overcurrent::setInput();

	// Enable VBUS Sensing B
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;

	// High-speed USB port
	usb_hs::Device::initialize<SystemClock>(priority);
	usb_hs::Device::connect<usb_hs::Dm::Dm, usb_hs::Dp::Dp, usb_hs::Id::Id>();
	usb_hs::Overcurrent::setInput();

	// Enable VBUS Sensing B
	USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}
/// @}

}
