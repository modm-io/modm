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
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nucleo_h7s3l8
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_h7s3l8
/// @{
using namespace modm::literals;

/// STM32H7S3L8 running at 600MHz from PLL1 generated from 24 MHz HSE
struct SystemClock
{
	static constexpr uint32_t Hse = 24_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll1
	{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 2,  //  24 MHz /  2 =  12 MHz
		.N = 50, //  12 MHz * 50 = 600 MHz
		.P = 1,  // 600 MHz /  1 = 600 MHz = F_cpu
		.Q = 4,  // 600 MHz /  4 = 150 MHz = SPI1/2/3
	};
	static constexpr uint32_t Pll1P = Hse / pll1.M * pll1.N / pll1.P;
	static constexpr uint32_t Pll1Q = Hse / pll1.M * pll1.N / pll1.Q;
	static_assert(Pll1P == Rcc::MaxFrequency);

	static constexpr Rcc::PllConfig pll3
	{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 3,  //  24 MHz /  3 =   8 MHz
		.N = 72, //   8 MHz * 72 = 576 MHz
		.Q = 12, // 576 MHz / 12 =  48 MHz = F_usb
	};
	static constexpr uint32_t Pll3Q = Hse / pll3.M * pll3.N / pll3.Q;

	static constexpr uint32_t SysClk = Pll1P;
	static constexpr uint32_t Frequency = SysClk;
	static constexpr uint32_t Cpu = SysClk;

	// AXI and AHB Buses - Max 300MHz
	static constexpr uint32_t Hclk = Cpu / 2;
	static constexpr uint32_t Axi = Hclk;
	static constexpr uint32_t Ahb1 = Hclk;
	static constexpr uint32_t Ahb2 = Hclk;
	static constexpr uint32_t Ahb3 = Hclk;
	static constexpr uint32_t Ahb4 = Hclk;
	static constexpr uint32_t Ahb5 = Hclk;

	// APB Buses - Max 150MHz
	static constexpr uint32_t Apb1 = Hclk / 2;
	static constexpr uint32_t Apb2 = Hclk / 2;
	static constexpr uint32_t Apb4 = Hclk / 2;
	static constexpr uint32_t Apb5 = Hclk / 2;

	// Kernel clocks with reset defaults
	static constexpr uint32_t Spi1 = Pll1Q;
	static constexpr uint32_t Spi2 = Pll1Q;
	static constexpr uint32_t Spi3 = Pll1Q;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb4;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;
	static constexpr uint32_t LpUart1 = Apb4;

	static constexpr uint32_t I2c1 = Apb1; // shared with I3C1
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;

	// Timer Clocks: x2 since APB prescaler is not 1
	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;

	// APB1 Timers
	static constexpr uint32_t Timer2 = Apb1Timer;
	static constexpr uint32_t Timer3 = Apb1Timer;
	static constexpr uint32_t Timer4 = Apb1Timer;
	static constexpr uint32_t Timer5 = Apb1Timer;
	static constexpr uint32_t Timer6 = Apb1Timer;
	static constexpr uint32_t Timer7 = Apb1Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	// APB2 Timers
	static constexpr uint32_t Timer1 = Apb2Timer;
	static constexpr uint32_t Timer9 = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = Pll3Q;
	static constexpr uint32_t UsbPhy = Hse;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		// The LDO supply is configured in init_ldo.cpp
		Rcc::setVoltageScaling(Rcc::VoltageScaling::High);

		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		Rcc::enablePll1(Rcc::PllSource::Hse, pll1);
		Rcc::enablePll3(Rcc::PllSource::Hse, pll3);

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div2);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb4Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb5Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setFlashLatency<Hclk>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setUsbClockSource(Rcc::UsbClockSource::Pll3Q);
		Rcc::setUsbPhyClockSource<UsbPhy>(Rcc::UsbPhyClockSource::Hse);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

// Arduino Uno footprint of the NUCLEO-H7S3L8 (UM3276 Tables 11-14)
using A0 = GpioA3;
using A1 = GpioC0;
using A2 = GpioC3;
using A3 = GpioC4;
using A4 = GpioC5;
using A5 = GpioF11;
using A6 = GpioB1;
using A7 = GpioC2;
using A8 = GpioF14;

using D0  = GpioA10;
using D1  = GpioB14;
using D2  = GpioF15;
using D3  = GpioE13;
using D4  = GpioF3;
using D5  = GpioE11;
using D6  = GpioE9;
using D7  = GpioF4;
using D8  = GpioF5;
using D9  = GpioD15;
using D10 = GpioD14;
using D11 = GpioB5;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB9;
using D15 = GpioB8;

using Button = GpioInverted<GpioInputC13>; // B2 is low when pressed

using LedGreen = GpioOutputD10;
using LedYellow = GpioOutputD13;
using LedRed = GpioOutputB7;
using Leds = SoftwareGpioPort< LedRed, LedYellow, LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_h7s3l8
/// @{
using Dm = GpioM5;
using Dp = GpioM6;

using Device = UsbHs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_h7s3l8
/// @{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = BufferedUart<UsartHal3, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_h7s3l8
/// @{
using Sda = D14;
using Scl = D15;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_h7s3l8
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_h7s3l8
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedYellow::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);

	Button::setInput();
}

inline void
initializeI2c()
{
	i2c::Controller::connect<i2c::Sda::Sda, i2c::Scl::Scl>();
	i2c::Controller::initialize<SystemClock, 400_kHz>();
}

inline void
initializeSpi()
{
	spi::Controller::connect<spi::Sck::Sck, spi::Sdo::Mosi, spi::Sdi::Miso>();
	spi::Controller::initialize<SystemClock, 4.6875_MHz>();
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

}
