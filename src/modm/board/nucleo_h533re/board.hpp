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

/// @ingroup modm_board_nucleo_h533re
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_h533re
/// @{
using namespace modm::literals;

/// STM32H533RE running at 250MHz from PLL clock generated from 24 MHz HSE
struct SystemClock
{
	static constexpr uint32_t Hse = 24_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll1
	{
		.range = Rcc::PllInputRange::MHz2_4,
		.M = 12,  //  24 MHz /  12 =   2 MHz
		.N = 250, //   2 MHz * 120 = 500 MHz
		.P = 2,   // 500 MHz /   2 = 250 MHz = F_cpu
	};
	static constexpr uint32_t Pll1P = Hse / pll1.M * pll1.N / pll1.P;
	static_assert(Pll1P == Rcc::MaxFrequency);

	static constexpr Rcc::PllConfig pll3
	{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 3,  //  24 MHz /  3 =   8 MHz
		.N = 18, //   8 MHz * 18 = 144 MHz
		.Q = 3,  // 144 MHz /  3 =  48 MHz = F_usb
	};
	static constexpr uint32_t Pll3Q = Hse / pll3.M * pll3.N / pll3.Q;

	static constexpr uint32_t SysClk = Pll1P;
	static constexpr uint32_t Frequency = SysClk;

	// AHB Bus - Max 250MHz
	static constexpr uint32_t Hclk = SysClk / 1;
	static constexpr uint32_t Ahb = Hclk;

	// APB Buses - Max 250MHz
	static constexpr uint32_t Apb1 = Hclk / 1;
	static constexpr uint32_t Apb2 = Hclk / 1;
	static constexpr uint32_t Apb3 = Hclk / 1;

	// Peripherals on AHB2
	static constexpr uint32_t Adc1 = Hclk;
	static constexpr uint32_t Dac1 = Hclk;

	// Peripherals on APB2
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;

	// Peripherals on APB1
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Fdcan1 = Apb1;

	// Peripherals on APB3
	static constexpr uint32_t LpUart1 = Apb3;

	// Timer Clocks
	static constexpr uint32_t Apb1Timer = Apb1 * 1;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;

	static constexpr uint32_t Timer1 = Apb2Timer;
	static constexpr uint32_t Timer2 = Apb1Timer;
	static constexpr uint32_t Timer3 = Apb1Timer;
	static constexpr uint32_t Timer6 = Apb1Timer;
	static constexpr uint32_t Timer7 = Apb1Timer;

	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = Pll3Q;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0);
		Rcc::setFlashLatency<Frequency>();

		Rcc::enablePll1(Rcc::PllSource::Hse, pll1);
		Rcc::enablePll3(Rcc::PllSource::Hse, pll3);

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::Pll3Q);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using A0 = GpioA0;
using A1 = GpioA1;
using A2 = GpioB1;
using A3 = GpioB0;
using A4 = GpioC1;
using A5 = GpioC0;

using D0  = GpioB15;
using D1  = GpioB14;
using D2  = GpioC8;
using D3  = GpioB3;
using D4  = GpioB5;
using D5  = GpioB4;
using D6  = GpioB10;
using D7  = GpioA8;
using D8  = GpioC7;
using D9  = GpioC6;
using D10 = GpioC9;
using D11 = GpioA7;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB7;
using D15 = GpioB6;

using Button = GpioInputC13;

using Led = GpioOutputA5;
using Leds = SoftwareGpioPort< Led >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_h533re
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_h533re
/// @{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_h533re
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Led::setOutput(modm::Gpio::Low);
	Button::setInput();
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

}
