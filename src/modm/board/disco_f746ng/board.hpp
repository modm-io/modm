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

#ifndef MODM_STM32_F746G_DISCOVERY_HPP
#define MODM_STM32_F746G_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_disco_f746ng
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_disco_f746ng
namespace Board
{
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

	static bool inline
	enable()
	{
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


using Button = GpioInputI11;	// User Button
using LedD13 = GpioOutputI1;	// User LED 1 (Arduino D13)

using Leds = SoftwareGpioPort< LedD13 >;

// Arduino footprint
using A0 = GpioA0;
using A1 = GpioF10;
using A2 = GpioF9;
using A3 = GpioF8;
using A4 = GpioF7;
using A5 = GpioF6;

using D0  = GpioC7;
using D1  = GpioC6;
using D2  = GpioG6;
using D3  = GpioB4;
using D4  = GpioG7;
using D5  = GpioA8;
using D6  = GpioH6;
using D7  = GpioI3;
using D8  = GpioI2;
using D9  = GpioA15;
using D10 = GpioI0;
using D11 = GpioB15;
using D12 = GpioB14;
using D13 = GpioI1;
using D14 = GpioB9;
using D15 = GpioB8;

namespace usb_fs
{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
}

namespace usb_hs
{
using Ck = GpioA5;

using D0 = GpioA3;
using D1 = GpioB0;
using D2 = GpioB1;
using D3 = GpioB10;
using D4 = GpioB11;
using D5 = GpioB12;
using D6 = GpioB13;
using D7 = GpioB5;

using Stp = GpioC0;
using Dir = GpioC2;
using Nxt = GpioH4;

using Overcurrent = GpioE3;

using Device = UsbHs;
}

namespace stlink
{
using Tx = GpioOutputA9;
using Rx = GpioInputB7;
using Uart = Usart1;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;


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

	// Disable Backlight
	GpioK3::setOutput(modm::Gpio::Low);
}

inline void
initializeUsbFs()
{
	usb_fs::Device::initialize<SystemClock>();
	usb_fs::Device::connect<usb_fs::Dm::Dm, usb_fs::Dp::Dp, usb_fs::Id::Id>();

	USB_OTG_DeviceTypeDef *dev = (USB_OTG_DeviceTypeDef *) (USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE);
	dev->DCTL |= USB_OTG_DCTL_SDIS;

	// Deactivate VBUS Sensing B
	USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

	// B-peripheral session valid override enable
	USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
	USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
}

inline void
initializeUsbHs()
{
	usb_hs::Device::initialize<SystemClock>();
	usb_hs::Device::connect<
		usb_hs::Ck::Ulpick,	usb_hs::Stp::Ulpistp,
		usb_hs::Dir::Ulpidir, usb_hs::Nxt::Ulpinxt,
		usb_hs::D0::Ulpid0, usb_hs::D1::Ulpid1,
		usb_hs::D2::Ulpid2, usb_hs::D3::Ulpid3,
		usb_hs::D4::Ulpid4, usb_hs::D5::Ulpid5,
		usb_hs::D6::Ulpid6, usb_hs::D7::Ulpid7>();
	usb_hs::Overcurrent::setInput();

	// Deactivate VBUS Sensing B
	USB_OTG_HS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

	// B-peripheral session valid override enable
	USB_OTG_HS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
	USB_OTG_HS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;

	// Force device mode
	USB_OTG_HS->GUSBCFG &= ~USB_OTG_GUSBCFG_FHMOD;
	USB_OTG_HS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;
}

}

#endif	// MODM_STM32_F746G_DISCOVERY_HPP
