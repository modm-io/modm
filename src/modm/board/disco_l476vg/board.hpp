// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2017-2018, 2021, Niklas Hauser
 * Copyright (c) 2018, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_L476_DISCOVERY_HPP
#define MODM_STM32_L476_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture.hpp>
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_l476vg
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_disco_l476vg
/// @{
using namespace modm::literals;

/// STM32L4 running at 80MHz generated from the
/// Internal Multispeed Oscillator
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr uint32_t Msi = 48_MHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 6,  //  48 MHz /  6 =   8 MHz
		.N = 40, //   8 MHz * 40 = 320 MHz
		.R = 4,  // 320 MHz /  4 =  80 MHz = F_cpu
		.Q = 4,  // 320 MHz /  4 =  80 MHz
	};
	static constexpr uint32_t PllR = Msi / pll.M * pll.N / pll.R;
	static_assert(PllR == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = PllR;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Ahb3 = Ahb;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Flash = Ahb1;
	static constexpr uint32_t Tsc = Ahb1;

	static constexpr uint32_t Adc1 = Ahb2;
	static constexpr uint32_t Adc2 = Ahb2;
	static constexpr uint32_t Adc3 = Ahb2;
	static constexpr uint32_t Adc123Common = Ahb2;
	static constexpr uint32_t Rng = Ahb2;
	static constexpr uint32_t Usbotg = Ahb2;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t Dac1 = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lcd = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Lptim2 = Apb1;
	static constexpr uint32_t Lpuart1 = Apb1;
	static constexpr uint32_t Opamp = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Swpmi1 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;
	static constexpr uint32_t Dfsdm1 = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Firewall = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;
	static constexpr uint32_t Sdmmc1 = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Vrefbuf = Apb2;

	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;
	static constexpr uint32_t Usb = Msi;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		// Disable the LSE Clock Security System
		RCC->BDCR &= ~RCC_BDCR_LSECSSON;

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);

		// The system already runs from the MSI
		Rcc::enableMsiClock(Rcc::MsiRange::MHz48);
		Rcc::enableMsiPllMode();
		Rcc::enablePll(Rcc::PllSource::Msi, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setClk48Source(Rcc::Clk48Source::Msi);

		return true;
	}
};

using Button = GpioInputA0;
/// @}

namespace Joystick
{
/// @ingroup modm_board_disco_l476vg
/// @{
using Left  = GpioInputA1;
using Right = GpioInputA2;
using Up    = GpioInputA3;
using Down  = GpioInputA5;
/// @}
}

/// @ingroup modm_board_disco_l476vg
/// @{
using LedRed   = GpioOutputB2;	// User LED 4
using LedGreen = GpioOutputE8;	// User LED 5

using Leds = SoftwareGpioPort< LedRed, LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_disco_l476vg
/// @{
using Overcurrent = GpioInputC10;	// OTG_FS_OverCurrent
using Vbus = GpioInputA9; // default PC11 if SB24 open
using Id = GpioA10; // default PC12 if SB25 left open
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_disco_l476vg
/// @{
using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);

	Button::setInput();
}

/// You must take out the LCD screen and close SB24 and SB25 for USB to work
inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();

	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}

/// @}

}

#endif	// MODM_STM32_L476_DISCOVERY_HPP
