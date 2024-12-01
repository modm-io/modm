/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
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

#include "rcc_prototype.hpp"

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_blue_pill_f103
/// @{
using namespace modm::literals;

struct SystemClock
{
	// static constexpr uint32_t ExternalLSEclock = ;
	static constexpr uint32_t ExternalHSEclock = 8_MHz;

	static constexpr Rcc::PllFactors pllFactors{
		.pllMul = 9,
		.usbPrediv = Rcc::UsbPrescaler::Div1_5
	};
	static constexpr Rcc::AhbPrescaler Ahb_prescaler = Rcc::AhbPrescaler::Div1;
	static constexpr Rcc::Apb1Prescaler Apb1_prescaler = Rcc::Apb1Prescaler::Div2;
	static constexpr Rcc::Apb2Prescaler Apb2_prescaler = Rcc::Apb2Prescaler::Div1;

	// ------------------------------------------

	static constexpr int HsePredivision = 1;
	static constexpr int PllClock = ExternalHSEclock / HsePredivision * pllFactors.pllMul; // 72 Mhz

	// System Clock MUX
	static constexpr uint32_t Clock = PllClock;
	static_assert(Clock <= 72_MHz, "Clock has max. 72MHz!");

	static constexpr uint32_t Ahb = Clock / RccProto::prescalerToValue<Ahb_prescaler>();
	static_assert(Ahb <= 72_MHz, "Ahb has max. 72MHz!");

	static constexpr uint32_t Apb1 = Ahb / RccProto::prescalerToValue<Apb1_prescaler>();
	static_assert(Apb1 <= 36_MHz, "Apb1 has max. 36MHz!");

	static constexpr uint32_t Apb2 = Ahb / RccProto::prescalerToValue<Apb2_prescaler>();
	static_assert(Apb2 <= 72_MHz, "Apb2 has max. 72MHz!");

	// @todo is this correct?
	static constexpr uint32_t Apb1Timer = Apb1 * (RccProto::prescalerToValue<Apb1_prescaler>() ==  1 ? 1 : 2);
	static constexpr uint32_t Apb2Timer = Apb2 * (RccProto::prescalerToValue<Apb2_prescaler>() ==  1 ? 1 : 2);

	// ------------------------------------------

	static constexpr uint32_t Frequency = Ahb;

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

	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;

	static constexpr uint32_t Usb = Ahb / RccProto::prescalerToValue<pllFactors.usbPrediv>(); // 48 MHz!
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);

		// set flash latency for 72MHz
		Rcc::setFlashLatency<Frequency>();

		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Ahb_prescaler);
		Rcc::setApb1Prescaler(Apb1_prescaler);
		Rcc::setApb2Prescaler(Apb2_prescaler);

		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};
/// @}

namespace usb
{
/// @ingroup modm_board_blue_pill_f103
/// @{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_blue_pill_f103
/// @{
using LedGreen = GpioInverted< GpioOutputC13 >;
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
initializeUsbFs(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

} // Board namespace
