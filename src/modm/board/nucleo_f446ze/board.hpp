/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_F446ZE_HPP
#define MODM_STM32_NUCLEO_F446ZE_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_f446ze
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_f446ze
namespace Board
{
	using namespace modm::literals;

/// STM32F446 running at 180 MHz from external 8 MHz STLink clock
struct SystemClock
{
	static constexpr uint32_t Frequency = 180_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc    = Apb2;

	static constexpr uint32_t Spi1   = Apb2;
	static constexpr uint32_t Spi2   = Apb1;
	static constexpr uint32_t Spi3   = Apb1;
	static constexpr uint32_t Spi4   = Apb2;
	static constexpr uint32_t Spi5   = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;
	static constexpr uint32_t I2c3   = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;

	static constexpr uint32_t Usb     = 48_MHz;

	static bool inline
	enable()
	{
		Rcc::enableExternalClock();	// 8MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 4,		//  8MHz / M=  4 ->   2MHz
			.pllN = 180,	//   2MHz * N=180 -> 360MHz
			.pllP = 2,		// 360MHz / P=  2 -> 180MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::ExternalClock, pllFactors);

		const Rcc::PllSaiFactors pllSaiFactors{
			.pllSaiM = 4,	//   8MHz / M=  4 ->   2MHz
			.pllSaiN = 96,	//   2MHz * N= 96 -> 192MHz
			.pllSaiP = 4,	// 192MHz / P=  4 ->  48MHz = F_usb
		};
		Rcc::enablePllSai(pllSaiFactors);

		// "Overdrive" is required for guaranteed stable 180 MHz operation
		Rcc::enableOverdriveMode();
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setClock48Source(Rcc::Clock48Source::PllSaiP);
		// APB1 has max. 45MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

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

namespace usb
{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputG7;	// OTG_FS_OverCurrent
using Power = GpioOutputG6;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
}

namespace stlink
{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = Usart3;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
	//  Button::enableExternalInterruptVector(12);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Force device mode
	USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
	modm::delay_ms(25);
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}

}

#endif  // MODM_STM32_NUCLEO_F446ZE_HPP
