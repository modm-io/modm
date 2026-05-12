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

/// @ingroup modm_board_nrf51422_dk
#define MODM_BOARD_HAS_LOGGER

/// @ingroup modm_board_nrf51422_dk
namespace Board
{
/// @ingroup modm_board_nrf51422_dk
/// @{
using namespace modm::literals;

struct SystemClock
{
	static constexpr uint32_t Frequency = 16_MHz;
	static constexpr uint32_t Uart0 = 16_MHz;
	static constexpr uint32_t Wdt = 32.768_kHz;

	static bool inline
	enable()
	{
		Hfcc::updateCoreFrequency<Frequency>();
		return true;
	}
};

using Led1 = GpioInverted<GpioOutputP0_21>;
using Led2 = GpioInverted<GpioOutputP0_22>;
using Led3 = GpioInverted<GpioOutputP0_23>;
using Led4 = GpioInverted<GpioOutputP0_24>;
using Leds = SoftwareGpioPort<Led4, Led3, Led2, Led1>;

using Button1 = GpioInverted<GpioInputP0_17>;
using Button2 = GpioInverted<GpioInputP0_18>;
using Button3 = GpioInverted<GpioInputP0_19>;
using Button4 = GpioInverted<GpioInputP0_20>;
using Buttons = SoftwareGpioPort<Button4, Button3, Button2, Button1>;

namespace uart
{
/// @ingroup modm_board_nrf51422_dk
/// @{
using Txd = GpioP0_9;
using Rxd = GpioP0_11;
using Uart = BufferedUart<UartHal0, UartTxBuffer<2048>, UartRxBuffer<128>>;
/// @}
}

/// @ingroup modm_board_nrf51422_dk
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
/// @}

} // namespace Board
