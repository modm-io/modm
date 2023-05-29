/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2021, Jeff McBride
 * Copyright (c) 2022, Christopher Durand
 * Copyright (c) 2023, Thomas Rush
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

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_feather_m4
/// @{
using namespace modm::literals;

/// The following pin aliases are as labeled on the board, with the following
/// exceptions:  Mosi is labeled "MO"; Miso is "MI"; and the 'D' pins
/// (D4-D6, D9-D13) do not have the "D" on the silkscreen, just the numbers
/// (except D4, go figure).

/// ARef (analog reference) is jumpered DIRECTLY (via a board trace)
/// to 3.3V, and therefore cannot be used for IO purposes.
using ARef	= GpioA03;

using A0	= GpioA02;
using A1	= GpioA05;
using A2	= GpioB08;
using A3	= GpioB09;
using A4	= GpioA04;
using A5	= GpioA06;

using Sck 	= GpioA17;
using Mosi 	= GpioB23;
using Miso 	= GpioB22;

using Rx	= GpioB17;
using Tx	= GpioB16;

using D13	= GpioA23;
using D12	= GpioA22;
using D11	= GpioA21;
using D10	= GpioA20;
using D9	= GpioA19;
using D6	= GpioA18;
using D5	= GpioA16;
using D4	= GpioA14;

using Scl	= GpioA13;
using Sda	= GpioA12;

/// This is the red LED by the USB jack.
using Led	= D13;

/// The "Neopixel" is Adafruit's name for a WS2812 equivalent RGB LED.
/// There is no breakout for it.
using Neopixel	= GpioB03;

/// Alternate names for Rx and Tx.
using D0	= Rx;
using D1	= Tx;

/// SAMD51J19A running at 120MHz generated from the 48MHz DFLL
/// divided by 48 for a 1MHz reference.
struct SystemClock
{
	static constexpr uint32_t Frequency = 120_MHz;

	static constexpr uint32_t Clock48MHz = 48_MHz;
	static constexpr auto Generator48MHz = ClockGenerator::Generator1;

	static constexpr uint32_t Clock32k = 32768;
	static constexpr auto Generator32k = ClockGenerator::Generator2;

	// when DFLL is in closed loop mode, the "1_MHz" DPLL reference is not
	// exactly 1MHz; calculating the actual value allows 'enableDpll()' to
	// check the tolerance
	static constexpr uint32_t Clock1MHz = ((48_MHz / 32768) * 32768) / 48;
	static constexpr auto Generator1MHz = ClockGenerator::Generator3;

	static constexpr uint32_t Usb = Clock48MHz;

	static constexpr uint32_t Sercom5 = Frequency;
	static constexpr uint32_t SercomSlow = Clock32k;

	static bool inline
	enable()
	{
		// start the 32kHz crystal oscillator and connect to generator
		GenericClockController::enableExternalCrystal32k(Xosc32StartupTime::Start_500ms);
		GenericClockController::enableGenerator<Generator32k, ClockSource::Xosc32k>();

		// set up Xosc32k as DFLL reference
		GenericClockController::connect<ClockPeripheral::Dfll48>(Generator32k);

		// while restarting DFLL, run system temporarily at 32kHz
		GenericClockController::enableGenerator<ClockGenerator::Main, ClockSource::Xosc32k>();

		// restart DFLL in closed loop
		GenericClockController::enableDfll48mClosedLoop<Clock32k>();

		// get system back up to speed
		GenericClockController::enableGenerator<ClockGenerator::Main, ClockSource::Dfll>();

		// set up a 1MHz reference for DPLL0
		GenericClockController::enableGenerator<Generator1MHz,
							GeneratorConfiguration{ClockSource::Dfll, 48U}>();
		GenericClockController::connect<ClockPeripheral::Fdpll0>(Generator1MHz);

		// start DPLL0
		GenericClockController::enableDpll<GclkSource<Clock1MHz>, Frequency>();

		// make it the system clock
		GenericClockController::setFlashLatency<Frequency>();
		GenericClockController::updateCoreFrequency<Frequency>();
		GenericClockController::setSystemClock<ClockSource::Dpll0>();

		GenericClockController::enableGenerator<Generator48MHz, ClockSource::Dfll>();

		GenericClockController::connect<ClockPeripheral::Usb>(Generator48MHz);

		GenericClockController::connect<ClockPeripheral::Sercom5>(ClockGenerator::System);
		GenericClockController::connect<ClockPeripheral::Sercom5Slow>(Generator32k);

		return true;
	}
};

using Leds = SoftwareGpioPort<Led>;

struct Debug
{
	using Uart = Uart5;
	using UartTx = Tx;
	using UartRx = Rx;
};

using LoggerDevice = modm::IODeviceWrapper<Debug::Uart, modm::IOBuffer::BlockIfFull>;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Debug::Uart::initialize<SystemClock, 115200>();
	Debug::Uart::connect<Debug::UartTx::Tx, Debug::UartRx::Rx>();

	Leds::setOutput(modm::Gpio::Low);

	// The Neopixel is normally set to black (off) by the bootloader after a
	// new program is uploaded to flash; but if a program is running and the
	// reset button is pressed while the Neopixel is lit, it will stay on with
	// the same color.
	//
	// Setting the output to low does not turn it off, it merely ensures that
	// the signal is in the idle state. Providing a driver in the board module
	// just to turn it off is unnecessary.
	Neopixel::setOutput(modm::Gpio::Low);
}

inline void
initializeUsbFs()
{
	modm::platform::Usb::initialize<SystemClock>();
	modm::platform::Usb::connect<GpioA24::Dm, GpioA25::Dp>();
}

/// @}

}  // namespace Board
