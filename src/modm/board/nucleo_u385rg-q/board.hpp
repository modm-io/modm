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

/// @ingroup modm_board_nucleo_u385rg_q
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_u385rg_q
/// @{
using namespace modm::literals;

/// STM32U385RG running at 96MHz from internal MSIS oscillator
struct SystemClock
{
	// LSE 32.768 kHz crystal is connected by default (SB1/SB2 ON)
	// HSE 16 MHz crystal is available but NOT connected by default
	// MCO 8 MHz from STLINK-V3EC is available but NOT connected by default
	static constexpr uint32_t Lse = 32.768_kHz;

	// MSIS (Multi Speed Internal System) clock frequency
	static constexpr uint32_t MsiS = 96_MHz;
	static_assert(MsiS == Rcc::MaxFrequency);

	static constexpr uint32_t SysClk = MsiS;
	static constexpr uint32_t Frequency = SysClk;

	static constexpr uint32_t Hclk = SysClk / 1;
	static constexpr uint32_t Ahb = Hclk;

	static constexpr uint32_t Apb = Hclk / 1;
	static constexpr uint32_t Apb1 = Apb;
	static constexpr uint32_t Apb2 = Apb;
	static constexpr uint32_t Apb3 = Apb;

	// AHB1 Peripherals
	static constexpr uint32_t Gpdma1 = Ahb;
	static constexpr uint32_t Flash = Ahb;
	static constexpr uint32_t Crc = Ahb;
	static constexpr uint32_t Tsc = Ahb;

	// AHB2 Peripherals
	static constexpr uint32_t Gpio = Ahb;
	static constexpr uint32_t Adc1 = Ahb;
	static constexpr uint32_t Adc2 = Ahb;
	static constexpr uint32_t Dac1 = Ahb;
	static constexpr uint32_t Aes = Ahb;
	static constexpr uint32_t Hash = Ahb;
	static constexpr uint32_t Rng = Ahb;

	// APB1 Peripherals
	static constexpr uint32_t Tim2 = Apb1;
	static constexpr uint32_t Tim3 = Apb1;
	static constexpr uint32_t Tim4 = Apb1;
	static constexpr uint32_t Tim6 = Apb1;
	static constexpr uint32_t Tim7 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I3c1 = Apb1;
	static constexpr uint32_t Opamp1 = Apb1;
	static constexpr uint32_t Opamp2 = Apb1;
	static constexpr uint32_t LpTimer2 = Apb1;
	static constexpr uint32_t FdCan1 = Apb1;

	// APB2 Peripherals
	static constexpr uint32_t Tim1 = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Tim15 = Apb2;
	static constexpr uint32_t Tim16 = Apb2;
	static constexpr uint32_t Tim17 = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t I3c2 = Apb2;

	// APB3 Peripherals
	static constexpr uint32_t LpUart1 = Apb3;
	static constexpr uint32_t I2c3 = Apb3;
	static constexpr uint32_t LpTimer1 = Apb3;
	static constexpr uint32_t LpTimer3 = Apb3;
	static constexpr uint32_t LpTimer4 = Apb3;
	static constexpr uint32_t Comp1 = Apb3;
	static constexpr uint32_t Comp2 = Apb3;

	// Timer Clocks (already defined above with correct bus assignments)

	// Special Clock Sources
	static constexpr uint32_t Rtc = Lse;
	static constexpr uint32_t Usb = 48_MHz;  // Requires HSI48
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHsiClock();
		Rcc::setBoosterClock(Rcc::BoosterClockSource::Hsi16, Rcc::BoosterPrescaler::Div1);

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range1_96MHz);
		Rcc::setFlashLatency<Frequency>();

		Rcc::enableMsiSystemClock(Rcc::MsiFrequency::MHz96);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::enableSystemClock(Rcc::SystemClockSource::MsiS);
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::enableHsi48Clock();
		Rcc::setUsbClockSource(Rcc::IclkSource::Hsi48);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		// Enable CRS clock
		RCC->APB1ENR1 |= RCC_APB1ENR1_CRSEN; __DSB();
		// Configure CRS: USB SOF sync, reload=47999, error limit=34, trim=32
		CRS->CFGR = (2U << CRS_CFGR_SYNCSRC_Pos) |         // USB SOF
					(34U << CRS_CFGR_FELIM_Pos) |          // Error limit
					(47999U << CRS_CFGR_RELOAD_Pos);       // Reload value
		CRS->CR = (32U << CRS_CR_TRIM_Pos) |               // Calibration value
				  CRS_CR_AUTOTRIMEN | CRS_CR_CEN;          // Enable auto-trim and CRS

		return true;
	}
};

using A0 = GpioA0;
using A1 = GpioA1;
using A2 = GpioA2;
using A3 = GpioB0;
using A4 = GpioC1;
using A5 = GpioC0;

using D0  = GpioB15;
using D1  = GpioB14;
using D2  = GpioA10;
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
/// @ingroup modm_board_nucleo_u385rg_q
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_u385rg_q
/// @{
using Tx = GpioOutputA9;
using Rx = GpioInputA10;
using Uart = BufferedUart<UsartHal1, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_u385rg_q
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
	// Button is active HIGH (LOW when released), requires pull-down
	Button::setInput(Gpio::InputType::PullDown);
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

}
