/*
 * Copyright (c) 2026, Niklas Hauser
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
#include <modm/architecture.hpp>
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nrf52840_dk
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nrf52840_dk
/// @{
using namespace modm::literals;

struct SystemClock
{
	static constexpr uint32_t Frequency = 64_MHz;
	static constexpr uint32_t Uarte0 = 16_MHz;
	static constexpr uint32_t Wdt = 32.768_kHz;

	static bool inline
	enable()
	{
		Hfcc::updateCoreFrequency<Frequency>();
		return true;
	}
};

using Led1 = GpioInverted<GpioOutputP0_13>;
using Led2 = GpioInverted<GpioOutputP0_14>;
using Led3 = GpioInverted<GpioOutputP0_15>;
using Led4 = GpioInverted<GpioOutputP0_16>;
using Leds = SoftwareGpioPort<Led4, Led3, Led2, Led1>;

using Button1 = GpioInverted<GpioInputP0_11>;
using Button2 = GpioInverted<GpioInputP0_12>;
using Button3 = GpioInverted<GpioInputP0_24>;
using Button4 = GpioInverted<GpioInputP0_25>;
using Buttons = SoftwareGpioPort<Button4, Button3, Button2, Button1>;
/// @}

namespace uart
{
/// @ingroup modm_board_nrf52840_dk
/// @{
using Txd = GpioP0_6;
using Rxd = GpioP0_8;
using Uart = BufferedUart<UarteHal0, UartTxBuffer<2048>, UartRxBuffer<128>>;
/// @}
}

namespace usb
{
/// @ingroup modm_board_nrf52840_dk
/// @{
using Dm = GpioP0_24;
using Dp = GpioP0_25;

using Device = Usb;
/// @}
}

/// @ingroup modm_board_nrf52840_dk
/// @{
using LoggerDevice = modm::IODeviceWrapper<uart::Uart, modm::IOBuffer::BlockIfFull>;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	uart::Uart::connect<uart::Txd::Txd, uart::Rxd::Rxd>();
	uart::Uart::initialize<SystemClock, 115200_Bd>();

	Leds::setOutput(modm::Gpio::Low);
	Buttons::setInput(Gpio::InputType::PullUp);
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm, usb::Dp>();
}
/// @}

} // namespace Board
