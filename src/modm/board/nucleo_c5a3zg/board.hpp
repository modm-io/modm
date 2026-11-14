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

/// @ingroup modm_board_nucleo_c5a3zg
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_c5a3zg
/// @{
using namespace modm::literals;

/// STM32C5A3ZG running at 144MHz from the PSI with the 48 MHz HSE crystal as reference
struct SystemClock
{
	static constexpr uint32_t Hse = 48_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PsiConfig psi
	{
		.source = Rcc::PsiSource::Hse,
		.reference = Rcc::PsiReference::MHz48,
		.frequency = Rcc::PsiFrequency::MHz144,
		.div3 = true, // 48 MHz for USB
	};
	static constexpr uint32_t Psis = 144_MHz;
	static constexpr uint32_t PsiDiv3 = Psis / 3;

	static constexpr uint32_t SysClk = Psis;
	static constexpr uint32_t Frequency = SysClk;
	static_assert(Frequency == Rcc::MaxFrequency);

	// AHB Buses
	static constexpr uint32_t Hclk = SysClk / 1;
	static constexpr uint32_t Ahb1 = Hclk;
	static constexpr uint32_t Ahb2 = Hclk;
	static constexpr uint32_t Ahb4 = Hclk;

	// APB Buses
	static constexpr uint32_t Apb1 = Hclk / 1;
	static constexpr uint32_t Apb2 = Hclk / 1;
	static constexpr uint32_t Apb3 = Hclk / 1;

	// Peripherals on AHB1
	static constexpr uint32_t Eth = Ahb1;

	// Peripherals on APB1
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Usart6 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I3c1 = Apb1;
	static constexpr uint32_t Fdcan1 = Apb1;
	static constexpr uint32_t Fdcan2 = Apb1;

	// Peripherals on APB2
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;

	// Peripherals on APB3
	static constexpr uint32_t LpUart1 = Apb3;
	static constexpr uint32_t LpTimer1 = Apb3;

	// Timer clocks are fixed to HCLK, independent of the APB prescalers (RM0514 11.5.14, RM0522 9.4.14)
	static constexpr uint32_t Apb1Timer = Hclk;
	static constexpr uint32_t Apb2Timer = Hclk;

	// APB1 Timers
	static constexpr uint32_t Timer2 = Apb1Timer;
	static constexpr uint32_t Timer3 = Apb1Timer;
	static constexpr uint32_t Timer4 = Apb1Timer;
	static constexpr uint32_t Timer5 = Apb1Timer;
	static constexpr uint32_t Timer6 = Apb1Timer;
	static constexpr uint32_t Timer7 = Apb1Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;

	// APB2 Timers
	static constexpr uint32_t Timer1 = Apb2Timer;
	static constexpr uint32_t Timer8 = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = PsiDiv3;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		Rcc::setFlashLatency<Frequency>();
		Rcc::enablePsi(psi);

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::Psi);
		Rcc::setClock48Source(Rcc::Clock48Source::PsiDiv3);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using Button = GpioInputC13;

using LedGreen = GpioOutputA5;
using LedRed = GpioInverted<GpioOutputG1>;
using LedBlue = GpioInverted<GpioOutputG2>;
using Leds = SoftwareGpioPort< LedBlue, LedRed, LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_c5a3zg
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

namespace eth
{
/// @ingroup modm_board_nucleo_c5a3zg
/// @{
using RefClk = GpioA1;
using Mdio = GpioE12;
using Mdc = GpioC1;
using CrsDv = GpioD1;
using Rxd0 = GpioC4;
using Rxd1 = GpioC5;
using TxEn = GpioG11;
using Txd0 = GpioG13;
using Txd1 = GpioG12;
using Interrupt = GpioG14;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_c5a3zg
/// @{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_c5a3zg
/// @{
using Sda = GpioB9;
using Scl = GpioB8;
using Controller = I2cMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_c5a3zg
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
	LedRed::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);

	Button::setInput();
}

inline void
initializeI2c()
{
	i2c::Controller::connect<i2c::Sda::Sda, i2c::Scl::Scl>();
	i2c::Controller::initialize<SystemClock, 400_kHz>();
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

}
