/*
 * Copyright (c) 2019, Niklas Hauser
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
/// @ingroup modm_board_nucleo_l152re
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_l152re
/// @{
using namespace modm::literals;

/// STM32L152RE running at 32MHz generated from the external 8MHz ST-Link clock
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	// VCO is 64 MHz, USB requires 96 MHz
	static constexpr Rcc::PllConfig pll{.Mul = Rcc::PllMultiplier::Mul8, .Div = 2};
	static constexpr uint32_t Pll = Hse * Rcc::value(pll.Mul) / pll.Div;
	static_assert(Pll == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = Pll;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Crc = Ahb;
	static constexpr uint32_t Dma1 = Ahb;
	static constexpr uint32_t Dma2 = Ahb;
	static constexpr uint32_t Flash = Ahb;

	static constexpr uint32_t Comp = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lcd = Apb1;
	static constexpr uint32_t Opamp = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t Ri = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;

	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;

	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseClock();

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range1);
		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);

		// The SYSCLK may increase at most 4x per step with 5us in between (RM0038 6.2.7)
		Rcc::enableSystemClock(Rcc::SystemClockSource::Hse);
		modm::delay_us(5);
		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

// Arduino Footprint
#include "nucleo64_arduino.hpp"

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_l152re
/// @{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_l152re
/// @{
using Sda = D14;
using Scl = D15;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_l152re
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_l152re
/// @{
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput();
}

inline void
initializeI2c()
{
	i2c::Controller::connect<i2c::Sda::Sda, i2c::Scl::Scl>();
	i2c::Controller::initialize<SystemClock, 427_kHz>();
}

inline void
initializeSpi()
{
	spi::Controller::connect<spi::Sck::Sck, spi::Sdo::Mosi, spi::Sdi::Miso>();
	spi::Controller::initialize<SystemClock, 4_MHz>();
}
/// @}

}
