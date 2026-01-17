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
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nucleo_u083rc
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_u083rc
/// @{
using namespace modm::literals;

/// STM32U083RC running at 56MHz from PLL clock generated from internal HSI
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll
	{
		.M = 2,  //  16 MHz /  2 =   8 MHz
		.N = 42, //   8 MHz * 42 = 336 MHz
		.Q = 7,  // 336 MHz /  7 =  48 MHz = F_usb
		.R = 6,  // 336 MHz /  6 =  56 MHz = F_cpu
	};
	static constexpr uint32_t PllQ = Rcc::HsiFrequency / pll.M * pll.N / pll.Q;
	static constexpr uint32_t PllR = Rcc::HsiFrequency / pll.M * pll.N / pll.R;
	static_assert(PllR == Rcc::MaxFrequency);

	static constexpr uint32_t SysClk = PllR;
	static constexpr uint32_t Frequency = SysClk;

	// Max 56MHz
	static constexpr uint32_t Hclk = SysClk / 1;
	static constexpr uint32_t Ahb = Hclk;
	static constexpr uint32_t Apb = Hclk / 1;

	// AHB Peripherals
	static constexpr uint32_t Dma1 = Ahb;
	static constexpr uint32_t Dma2 = Ahb;
	static constexpr uint32_t DmaMux = Ahb;
	static constexpr uint32_t Exti = Ahb;
	static constexpr uint32_t Flash = Ahb;
	static constexpr uint32_t Crc = Ahb;
	static constexpr uint32_t Tsc = Ahb;
	static constexpr uint32_t Rng = Ahb;
	static constexpr uint32_t Aes = Ahb;

	// APB Peripherals
	static constexpr uint32_t Lcd = Apb;
	static constexpr uint32_t Wwdg = Apb;
	static constexpr uint32_t Spi1 = Apb;
	static constexpr uint32_t Spi2 = Apb;
	static constexpr uint32_t Spi3 = Apb;
	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;
	static constexpr uint32_t Usart3 = Apb;
	static constexpr uint32_t Usart4 = Apb;
	static constexpr uint32_t I2c1 = Apb;
	static constexpr uint32_t I2c2 = Apb;
	static constexpr uint32_t I2c3 = Apb;
	static constexpr uint32_t I2c4 = Apb;
	static constexpr uint32_t Dac1 = Apb;
	static constexpr uint32_t Opamp1 = Apb;
	static constexpr uint32_t LpTimer1 = Apb;
	static constexpr uint32_t LpTimer2 = Apb;
	static constexpr uint32_t LpTimer3 = Apb;
	static constexpr uint32_t LpUart1 = Apb;
	static constexpr uint32_t LpUart2 = Apb;
	static constexpr uint32_t LpUart3 = Apb;
	static constexpr uint32_t Adc1 = Apb;

	// Timer Clocks
	static constexpr uint32_t ApbTimer = Apb * 1;
	static constexpr uint32_t Timer1 = ApbTimer;
	static constexpr uint32_t Timer2 = ApbTimer;
	static constexpr uint32_t Timer3 = ApbTimer;
	static constexpr uint32_t Timer6 = ApbTimer;
	static constexpr uint32_t Timer7 = ApbTimer;
	static constexpr uint32_t Timer15 = ApbTimer;
	static constexpr uint32_t Timer16 = ApbTimer;

	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = PllQ;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHsiClock();

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range1);
		Rcc::setFlashLatency<Frequency>();

		Rcc::enablePll(Rcc::PllSource::Hsi16, pll);

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApbPrescaler(Rcc::ApbPrescaler::Div1);

		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableSystemClock(Rcc::SystemClockSource::PllR);
		Rcc::setClock48Source(Rcc::Clock48Source::PllQ);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using A0 = GpioA0;
using A1 = GpioA1;
using A2 = GpioA2;
using A3 = GpioB0;
using A4 = GpioC1;
using A5 = GpioC0;

// using D0  = GpioA3; // used for logging
// using D1  = GpioA2;
using D2  = GpioA10;
using D3  = GpioB3;
using D4  = GpioB5;
using D5  = GpioB4;
using D6  = GpioB10;
using D7  = GpioA8;
using D8  = GpioA9;
using D9  = GpioC7;
using D10 = GpioB6;
using D11 = GpioA7;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB9;
using D15 = GpioB8;

using Button = GpioInverted<GpioInputC13>;

using Led = GpioOutputA5;
using Leds = SoftwareGpioPort< Led >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_u083rc
/// @{
using Tx = GpioOutputA2;
using Rx = GpioInputA3;
using Uart = BufferedUart<UsartHal2, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_u083rc
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
	Button::setInput(Gpio::InputType::PullUp);
}
/// @}

}
