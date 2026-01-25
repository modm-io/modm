/*
 *
 * Copyright (c) 2018, Nick Sarten
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F103C8T6_BLACK_PILL_HPP
#define MODM_STM32_F103C8T6_BLACK_PILL_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_black_pill_f103
/// @{
using namespace modm::literals;

/// STM32F103 running at 72MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr Rcc::PllConfig pll{.mul = 9, .usbdiv = Rcc::UsbPrescaler::Div1_5};
	static constexpr uint32_t Pll = Hse * pll.mul;
	static constexpr uint32_t Frequency = Pll;
	static_assert(Frequency == Rcc::MaxFrequency);

	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc  = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;

	static constexpr uint32_t Can    = Apb1;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;

	static constexpr uint32_t Usb = Ahb * 2 / 3;
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
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};
/// @}

namespace usb
{
/// @ingroup modm_board_black_pill_f103
/// @{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_black_pill_f103
/// @{
using LedGreen = GpioInverted< GpioOutputB12 >;
using Leds = SoftwareGpioPort< LedGreen >;

using Button = GpioUnused;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}

// DEPRECATE: 2026q2
[[deprecated("Use initializeUsb() instead!")]]
inline void
initializeUsbFs(uint8_t priority=3)
{ initializeUsb(priority); }
/// @}

} // Board namespace

#endif	// MODM_STM32_F103C8T6_BLACK_PILL_HPP
