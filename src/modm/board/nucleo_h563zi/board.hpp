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

/// @ingroup modm_board_nucleo_h563zi
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_h563zi
/// @{
using namespace modm::literals;

/// STM32H563ZI running at 250MHz from PLL clock generated from the default 8 MHz ST-LINK MCO
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll1
	{
		.range = Rcc::PllInputRange::MHz2_4,
		.M = 4,   //   8 MHz /   4 =   2 MHz
		.N = 250, //   2 MHz * 250 = 500 MHz
		.P = 2,   // 500 MHz /   2 = 250 MHz = F_cpu
	};
	static constexpr uint32_t Pll1P = Hse / pll1.M * pll1.N / pll1.P;
	static_assert(Pll1P == Rcc::MaxFrequency);

	static constexpr Rcc::PllConfig pll3
	{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 1,  //   8 MHz /  1 =   8 MHz
		.N = 18, //   8 MHz * 18 = 144 MHz
		.Q = 3,  // 144 MHz /  3 =  48 MHz = F_usb
	};
	static constexpr uint32_t Pll3Q = Hse / pll3.M * pll3.N / pll3.Q;

	static constexpr uint32_t SysClk = Pll1P;
	static constexpr uint32_t Frequency = SysClk;

	// AHB Buses - Max 250MHz
	static constexpr uint32_t Hclk = SysClk / 1;
	static constexpr uint32_t Ahb1 = Hclk;
	static constexpr uint32_t Ahb2 = Hclk;
	static constexpr uint32_t Ahb3 = Hclk;
	static constexpr uint32_t Ahb4 = Hclk;

	// APB Buses - Max 250MHz
	static constexpr uint32_t Apb1 = Hclk / 1;
	static constexpr uint32_t Apb2 = Hclk / 1;
	static constexpr uint32_t Apb3 = Hclk / 1;

	// Peripherals on AHB2
	static constexpr uint32_t Adc1 = Ahb2;
	static constexpr uint32_t Adc2 = Ahb2;
	static constexpr uint32_t Dac1 = Ahb2;

	// Peripherals on AHB4
	static constexpr uint32_t Sdmmc1 = Ahb4;
	static constexpr uint32_t Fmc = Ahb4;
	static constexpr uint32_t OctoSpi1 = Ahb4;
	static constexpr uint32_t Eth = Ahb4;

	// Peripherals on APB2
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi6 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;

	// Peripherals on APB1
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Usart6 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;
	static constexpr uint32_t Uart9 = Apb1;
	static constexpr uint32_t Usart10 = Apb1;
	static constexpr uint32_t Usart11 = Apb1;
	static constexpr uint32_t Uart12 = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t Fdcan1 = Hse;

	// Peripherals on APB3
	static constexpr uint32_t Spi5 = Apb3;
	static constexpr uint32_t LpUart1 = Apb3;
	static constexpr uint32_t I2c3 = Apb3;
	static constexpr uint32_t I2c4 = Apb3;

	// Timer Clocks
	static constexpr uint32_t Apb1Timer = Apb1 * 1;
	static constexpr uint32_t Apb2Timer = Apb2 * 1;

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
	static constexpr uint32_t Timer8 = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = Pll3Q;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseClock();

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

// Arduino Footprint
#include "nucleo144_arduino_h5.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;
using LedYellow = GpioOutputF4;
using LedRed = GpioOutputG4;
using Leds = SoftwareGpioPort< LedRed, LedYellow, LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_h563zi
/// @{
using Vbus = GpioInputA4;
using Dm = GpioA11;
using Dp = GpioA12;

using UcpdDBn = GpioA9;
using UcpdCc1 = GpioB13;
using UcpdCc2 = GpioB14;
using UcpdFlt = GpioG7;

using Device = UsbFs;
/// @}
}

namespace eth
{
/// @ingroup modm_board_nucleo_h563zi
/// @{
using RefClk = GpioA1;
using Mdio = GpioA2;
using Mdc = GpioC1;
using CrsDv = GpioA7;
using Rxd0 = GpioC4;
using Rxd1 = GpioC5;
using TxEn = GpioG11;
using Txd0 = GpioG13;
using Txd1 = GpioB15;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_h563zi
/// @{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = BufferedUart<UsartHal3, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_h563zi
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
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
	usb::Vbus::setInput();
}
/// @}

}