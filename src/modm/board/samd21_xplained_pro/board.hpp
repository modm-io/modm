/*
 * Copyright (c) 2022, Christopher Durand
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
/// @ingroup modm_board_samd21_xplained_pro
/// @{
using namespace modm::literals;
using namespace modm::platform;


struct SystemClock
{
	static constexpr uint32_t Dfll48m = 48_MHz;
	static constexpr uint32_t Xosc32k = 32768_Hz;

	static constexpr uint32_t Frequency = Dfll48m;
	static constexpr uint32_t Usb = Dfll48m;

	static constexpr auto ClockGen32kHz = ClockGenerator::Generator2;

	static constexpr uint32_t Sercom3 = Frequency;
	static constexpr uint32_t SercomSlow = Xosc32k;

	static bool inline
	enable()
	{
		// Configure GCLK generator 2 with external 32k crystal source
		GenericClockController::enableExternalCrystal32k(Xosc32StartupTime::Start_500ms);
		GenericClockController::enableGenerator<ClockGen32kHz, ClockSource::Xosc32k>();

		// generate 48 MHz from 32768 Hz crystal reference
		GenericClockController::connect<ClockPeripheral::Dfll48>(ClockGen32kHz);
		GenericClockController::enableDfll48mClosedLoop<Xosc32k>();

		GenericClockController::setFlashLatency<Frequency>();
		GenericClockController::setSystemClock<ClockSource::Dfll48m>();
		GenericClockController::updateCoreFrequency<Frequency>();

		GenericClockController::connect<ClockPeripheral::Usb>(ClockGenerator::System);

		GenericClockController::connect<ClockPeripheral::Sercom3>(ClockGenerator::System);
		GenericClockController::connect<ClockPeripheral::Sercom3Slow>(ClockGen32kHz);

		return true;
	}
};

using Led0 = GpioB30;
using Button = GpioA15;

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

	static void toggle()
	{
		Led0::toggle();
	}
};

struct Debug
{
	using Uart = Uart3;
	using UartTx = GpioA22;
	using UartRx = GpioA23;
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

	Button::setInput(Button::InputType::PullUp);
}

inline void initializeUsbFs()
{
	modm::platform::Usb::initialize<Board::SystemClock>();
	modm::platform::Usb::connect<GpioA24::Dm, GpioA25::Dp>();
}
/// @}

} // namespace Board

