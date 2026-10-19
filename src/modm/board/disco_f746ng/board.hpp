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

namespace Board
{
/// @ingroup modm_board_disco_f746ng
/// @{
using namespace modm::literals;

/// STM32F7 running at 216MHz from the external 25MHz clock
struct SystemClock
{
	static constexpr uint32_t Hse = 25_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 25,  //  25 MHz /  25 =   1 MHz
		.N = 432, //   1 MHz * 432 = 432 MHz
		.P = 2,   // 432 MHz /   2 = 216 MHz = F_cpu
		.Q = 9,   // 432 MHz /   9 =  48 MHz = F_usb
	};
	static constexpr uint32_t PllP = Hse / pll.M * pll.N / pll.P;
	static constexpr uint32_t PllQ = Hse / pll.M * pll.N / pll.Q;
	static_assert(PllP == Rcc::MaxFrequency);
	static_assert(PllQ == 48_MHz);

	static constexpr uint32_t Frequency = PllP;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Dma2d = Ahb1;
	static constexpr uint32_t Eth = Ahb1;
	static constexpr uint32_t Flash = Ahb1;

	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Rng = Ahb2;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;
	static constexpr uint32_t Cec = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t SpdifRx = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Adc2 = Apb2;
	static constexpr uint32_t Adc3 = Apb2;
	static constexpr uint32_t Adc123Common = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Ltdc = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;
	static constexpr uint32_t Sdmmc1 = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart6 = Apb2;

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
	static constexpr uint32_t Rtc = Lse;

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
		// Required for 216 MHz operation
		Rcc::enableOverdriveMode();
		Rcc::enableSystemClock(Rcc::SystemClockSource::PllP);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

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
/// @}

namespace usb_fs
{
/// @ingroup modm_board_disco_f746ng
/// @{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

namespace usb_hs
{
/// @ingroup modm_board_disco_f746ng
/// @{
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
/// @}
}

namespace stlink
{
/// @ingroup modm_board_disco_f746ng
/// @{
using Tx = GpioOutputA9;
using Rx = GpioInputB7;
using Uart = BufferedUart<UsartHal1, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_disco_f746ng
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

	// Disable Backlight
	GpioK3::setOutput(modm::Gpio::Low);
}

inline void
initializeUsb(uint8_t priority=3)
{
	// Full-speed USB port
	usb_fs::Device::initialize<SystemClock>(priority);
	usb_fs::Device::connect<usb_fs::Dm::Dm, usb_fs::Dp::Dp, usb_fs::Id::Id>();

	USB_OTG_DeviceTypeDef *dev = (USB_OTG_DeviceTypeDef *) (USB_OTG_FS_PERIPH_BASE + USB_OTG_DEVICE_BASE);
	dev->DCTL |= USB_OTG_DCTL_SDIS;

	// Deactivate VBUS Sensing B
	USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_VBDEN;

	// B-peripheral session valid override enable
	USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;
	USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;

	// High-speed USB port
	usb_hs::Device::initialize<SystemClock>(priority);
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


/// @}

}

#endif	// MODM_STM32_F746G_DISCOVERY_HPP
