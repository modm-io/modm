/*
 * Copyright (c) 2021-2022, Christopher Durand
 * Copyright (c) 2021, Jeff McBride
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
#include <modm/debug/logger.hpp>

#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_same54_xplained_pro
/// @{
using namespace modm::literals;
using namespace modm::platform;


struct SystemClock
{
	static constexpr uint32_t Frequency = 120_MHz;

	static constexpr uint32_t Clock48MHz = 48_MHz;
	static constexpr auto Generator48MHz = ClockGenerator::Generator1;

	static constexpr uint32_t Clock32k = 32768;
	static constexpr auto Generator32k = ClockGenerator::Generator2;

	static constexpr uint32_t Usb = Clock48MHz;

	static constexpr uint32_t Sercom2 = Frequency;
	static constexpr uint32_t SercomSlow = Clock32k;

	static bool inline
	enable()
	{
		GenericClockController::enableExternalCrystal<12_MHz>(Xosc::Xosc1);
		GenericClockController::enableDpll<Xosc1Source<12_MHz>, 120_MHz>();

		GenericClockController::setFlashLatency<Frequency>();
		GenericClockController::updateCoreFrequency<Frequency>();
		GenericClockController::setSystemClock<ClockSource::Dpll0>();

		GenericClockController::enableExternalCrystal32k(Xosc32StartupTime::Start_500ms);
		GenericClockController::enableGenerator<Generator32k, ClockSource::Xosc32k>();

		// generate 48 MHz clock from external 32768 Hz crystal reference
		GenericClockController::connect<ClockPeripheral::Dfll48>(Generator32k);
		GenericClockController::enableDfll48mClosedLoop<Clock32k>();

		GenericClockController::enableGenerator<Generator48MHz, ClockSource::Dfll>();
		GenericClockController::connect<ClockPeripheral::Usb>(Generator48MHz);

		GenericClockController::connect<ClockPeripheral::Sercom2>(ClockGenerator::System);
		GenericClockController::connect<ClockPeripheral::Sercom2Slow>(Generator32k);

		return true;
	}
};

using Led0 = GpioC18;
using Button = GpioB31;

// No SoftwareGpioPort yet for SAM
struct Leds
{
	static constexpr std::size_t width{1};

	static void setOutput()
	{
		Led0::setOutput();
	}

	static void write(uint32_t value)
	{
		Led0::set(value & 1);
	}
};

struct Debug
{
	using Uart = Uart2;
	using UartTx = GpioB25;
	using UartRx = GpioB24;
};

using LoggerDevice = modm::IODeviceWrapper<Debug::Uart, modm::IOBuffer::BlockIfFull>;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Debug::Uart::initialize<SystemClock, 115200>();
	Debug::Uart::connect<Debug::UartTx::Tx, Debug::UartRx::Rx>();

	Led0::setOutput(modm::Gpio::Low);

	Button::setInput(InputType::PullUp);
}

inline void initializeUsbFs()
{
	modm::platform::Usb::initialize<Board::SystemClock>();
	modm::platform::Usb::connect<GpioA24::Dm, GpioA25::Dp>();
}
/// @}

} // namespace Board

