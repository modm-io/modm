/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_STM32_NUCLEO_G071RB_HPP
#define MODM_STM32_NUCLEO_G071RB_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_g071rb
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_nucleo_g071rb
namespace Board
{
using namespace modm::literals;

/// STM32G071RB running at 64MHz generated from the internal 16MHz crystal
// Dummy clock for devices
struct SystemClock {
	static constexpr uint32_t Frequency = 64_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Aes		= Ahb;
	static constexpr uint32_t Rng		= Ahb;
	static constexpr uint32_t Crc		= Ahb;
	static constexpr uint32_t Flash		= Ahb;
	static constexpr uint32_t Exti		= Ahb;
	static constexpr uint32_t Rcc		= Ahb;
	static constexpr uint32_t Dmamux	= Ahb;
	static constexpr uint32_t Dma		= Ahb;

	static constexpr uint32_t Dbg		= Apb;
	static constexpr uint32_t Timer17	= Apb;
	static constexpr uint32_t Timer16	= Apb;
	static constexpr uint32_t Timer15	= Apb;
	static constexpr uint32_t Usart1	= Apb;
	static constexpr uint32_t Spi1		= Apb;
	static constexpr uint32_t I2s1		= Apb;
	static constexpr uint32_t Timer1	= Apb;
	static constexpr uint32_t Adc1		= Apb;
	static constexpr uint32_t Comp		= Apb;
	static constexpr uint32_t ItLine	= Apb;
	static constexpr uint32_t VrefBuf	= Apb;
	static constexpr uint32_t SysCfg	= Apb;
	static constexpr uint32_t Tamp		= Apb;
	static constexpr uint32_t Bkp		= Apb;
	static constexpr uint32_t Ucpd2		= Apb;
	static constexpr uint32_t Ucpd1		= Apb;
	static constexpr uint32_t LpTimer2	= Apb;
	static constexpr uint32_t LpUart1	= Apb;
	static constexpr uint32_t LpTimer1	= Apb;
	static constexpr uint32_t HdmiCec	= Apb;
	static constexpr uint32_t Dac		= Apb;
	static constexpr uint32_t Pwr		= Apb;
	static constexpr uint32_t I2c2		= Apb;
	static constexpr uint32_t I2c1		= Apb;
	static constexpr uint32_t Usart4	= Apb;
	static constexpr uint32_t Usart3	= Apb;
	static constexpr uint32_t Usart2	= Apb;
	static constexpr uint32_t Spi2		= Apb;
	static constexpr uint32_t Iwdg		= Apb;
	static constexpr uint32_t Wwdg		= Apb;
	static constexpr uint32_t Rtc		= Apb;
	static constexpr uint32_t Timer14	= Apb;
	static constexpr uint32_t Timer7	= Apb;
	static constexpr uint32_t Timer6	= Apb;
	static constexpr uint32_t Timer3	= Apb;
	static constexpr uint32_t Timer2	= Apb;

	static bool inline
	enable()
	{
		Rcc::enableInternalClock();	// 16MHz
		// (internal clock / 1_M) * 8_N / 2_R = 128MHz / 2 = 64MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 1,
			.pllN = 8,
			.pllR = 2,
		};
		Rcc::enablePll(Rcc::PllSource::InternalClock, pllFactors);
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

// Arduino Footprint
using A0 = GpioA0;
using A1 = GpioA1;
using A2 = GpioA4;
using A3 = GpioB1;
using A4 = GpioB11;
using A5 = GpioB12;

using D0  = GpioC5;
using D1  = GpioC4;
using D2  = GpioA10;
using D3  = GpioB3;
using D4  = GpioB5;
using D5  = GpioB4;
using D6  = GpioB14;
using D7  = GpioA8;
using D8  = GpioA9;
using D9  = GpioC7;
using D10 = GpioB0;
using D11 = GpioA7;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB9;
using D15 = GpioB8;

using Button = GpioInverted<GpioInputC13>;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;


namespace stlink
{
using Rx = GpioInputA3;
using Tx = GpioOutputA2;
using Uart = Usart2;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// MODM_STM32_NUCLEO_G071RB_HPP
