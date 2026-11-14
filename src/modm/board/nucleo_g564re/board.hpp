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

/// @ingroup modm_board_nucleo_g564re
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_g564re
/// @{
using namespace modm::literals;

/// STM32G564RE running at 280MHz from PLL1 generated from the 24 MHz HSE crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 24_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll1
	{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 3,   //  24 MHz /  3 =   8 MHz
		.N = 70,  //   8 MHz * 70 = 560 MHz
		.P = 2,   // 560 MHz /  2 = 280 MHz = F_cpu
		.Q = 7,   // 560 MHz /  7 =  80 MHz = F_fdcan
	};
	static constexpr uint32_t Pll1P = Hse / pll1.M * pll1.N / pll1.P;
	static constexpr uint32_t Pll1Q = Hse / pll1.M * pll1.N / pll1.Q;
	static_assert(Pll1P == Rcc::MaxFrequency);

	static constexpr uint32_t SysClk = Pll1P;
	static constexpr uint32_t Frequency = SysClk;

	// AHB Buses
	static constexpr uint32_t Hclk = SysClk / 1;
	static constexpr uint32_t Ahb1 = Hclk;
	static constexpr uint32_t Ahb2 = Hclk;
	static constexpr uint32_t Ahb4 = Hclk;

	// APB Buses
	static constexpr uint32_t Apb1 = Hclk / 1;
	static constexpr uint32_t Apb2 = Hclk / 1;
	static constexpr uint32_t Apb3 = Hclk / 1;

	// Peripherals on APB1
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Uart6 = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I3c1 = Apb1;
	static constexpr uint32_t Fdcan1 = Hse;
	static constexpr uint32_t Fdcan2 = Hse;
	static constexpr uint32_t Fdcan3 = Hse;

	// Peripherals on APB2
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;

	// Peripherals on APB3
	static constexpr uint32_t LpUart1 = Apb3;
	static constexpr uint32_t I2c3 = Apb3;
	static constexpr uint32_t I2c4 = Apb3;
	static constexpr uint32_t LpTimer1 = Apb3;

	// Timer clocks are fixed to HCLK, independent of the APB prescalers (RM0514 11.5.14, RM0522 9.4.14)
	static constexpr uint32_t Apb1Timer = Hclk;
	static constexpr uint32_t Apb2Timer = Hclk;
	static constexpr uint32_t Apb3Timer = Hclk;

	// APB1 Timers
	static constexpr uint32_t Timer2 = Apb1Timer;
	static constexpr uint32_t Timer3 = Apb1Timer;
	static constexpr uint32_t Timer6 = Apb1Timer;
	static constexpr uint32_t Timer7 = Apb1Timer;
	static constexpr uint32_t Timer9 = Apb1Timer;
	static constexpr uint32_t Timer10 = Apb1Timer;
	static constexpr uint32_t Timer11 = Apb1Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	// APB2 Timers
	static constexpr uint32_t Timer1 = Apb2Timer;
	static constexpr uint32_t Timer4 = Apb2Timer;
	static constexpr uint32_t Timer5 = Apb2Timer;
	static constexpr uint32_t Timer8 = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	// APB3 Timers
	static constexpr uint32_t Timer20 = Apb3Timer;
	static constexpr uint32_t Timer26 = Apb3Timer;
	static constexpr uint32_t Timer27 = Apb3Timer;

	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = 48_MHz; // HSI48
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();
		Rcc::enableHsi48Clock();

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0);
		Rcc::setFlashLatency<Frequency, 1300>();

		Rcc::enablePll1(Rcc::PllSource::Hse, pll1);

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::Hsi48);
		Rcc::setCanClockSource(Rcc::CanClockSource::Hse);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using Button = GpioInputC13;

using LedGreen = GpioOutputG3;
using Leds = SoftwareGpioPort< LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_g564re
/// @{
using Vbus = GpioInputA4;
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

namespace can
{
/// @ingroup modm_board_nucleo_g564re
/// @{
using Rx = GpioInputB12;
using Tx = GpioOutputB13;
using Standby = GpioOutputB5;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_g564re
/// @{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_g564re
/// @{
using Sda = GpioB9;
using Scl = GpioA15;
using Controller = I2cMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_g564re
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
	usb::Vbus::setInput();
}
/// @}

}
