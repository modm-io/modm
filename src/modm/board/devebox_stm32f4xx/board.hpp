/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F4XX_DEVEBOX_HPP
#define MODM_STM32_F4XX_DEVEBOX_HPP

#include <modm/platform.hpp>
#include <modm/architecture.hpp>
#include <modm/debug.hpp>
#include <modm/driver/storage/block_device_spiflash.hpp>

using namespace modm::platform;

/// @ingroup modm_board_devebox_f4xx
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_devebox_f4xx
/// @{
using namespace modm::literals;

/// STM32F407 running at 168MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 4,   //   8 MHz /   4 =   2 MHz
		.N = 168, //   2 MHz * 168 = 336 MHz
		.P = 2,   // 336 MHz /   2 = 168 MHz = F_cpu
		.Q = 7,   // 336 MHz /   7 =  48 MHz
	};
	static constexpr uint32_t PllP = Hse / pll.M * pll.N / pll.P;
	static constexpr uint32_t PllQ = Hse / pll.M * pll.N / pll.Q;
	static_assert(PllP == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = PllP;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Eth = Ahb1;
	static constexpr uint32_t Flash = Ahb1;

	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Rng = Ahb2;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2s2Ext = Apb1;
	static constexpr uint32_t I2s3Ext = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Adc2 = Apb2;
	static constexpr uint32_t Adc3 = Apb2;
	static constexpr uint32_t Adc123Common = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Sdio = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart6 = Apb2;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer9  = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	static constexpr uint32_t Usb = PllQ;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);

		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::PllP);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};


using Button = GpioInputA0;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using LedGreen = GpioInverted<GpioOutputA1>;	// User LED

using Leds = SoftwareGpioPort< LedGreen >;

using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;
/// @}

namespace usb
{
/// @ingroup modm_board_devebox_f4xx
/// @{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
/// @}
}

namespace w25q16
{
/// @ingroup modm_board_devebox_f4xx
/// @{
using Cs   = GpioOutputA15;
using Sck  = GpioOutputB3;
using Mosi = GpioOutputB5;
using Miso = GpioInputB4;

using SpiMaster = SpiMaster1;

constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 2*1024*1024; // 16 MiBits
using StorageDevice = modm::BdSpiFlash<SpiMaster, Cs, MemorySize>;
/// @}
}

/// @ingroup modm_board_devebox_f4xx
/// @{
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);

	Button::setInput(Gpio::InputType::PullDown);
}


inline void
initializeW25q16()
{
	w25q16::Cs::setOutput(modm::Gpio::High);

	w25q16::SpiMaster::connect<w25q16::Sck::Sck, w25q16::Mosi::Mosi, w25q16::Miso::Miso>();
	w25q16::SpiMaster::initialize<SystemClock, 42_MHz>();
}
/// @}

}

#endif	// MODM_STM32_F4XX_DEVEBOX_HPP
