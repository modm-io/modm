/*
 * Copyright (c) 2015-2018, Niklas Hauser
 * Copyright (c) 2016, Fabian Greif
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2018, Antal Szabó
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F769I_DISCOVERY_HPP
#define MODM_STM32_F769I_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_disco_f769ni
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;


namespace Board
{
/// @ingroup modm_board_disco_f769ni
/// @{
using namespace modm::literals;

/// STM32F7 running at 216MHz from the external 25MHz clock
struct SystemClock
{
	static constexpr uint32_t Hse = 25_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 25,  //  25 MHz /  25 =   1 MHz
		.N = 432, //   1 MHz * 432 = 432 MHz
		.P = 2,   // 432 MHz /   2 = 216 MHz = F_cpu
		.Q = 9,   // 432 MHz /   9 =  48 MHz = F_usb
	};
	static constexpr uint32_t PllP = Hse / pll.M * pll.N / pll.P;
	static constexpr uint32_t PllQ = Hse / pll.M * pll.N / pll.Q;
	static_assert(PllP == Rcc::MaxFrequency);
	static_assert(PllQ == 48_MHz);

	static constexpr uint32_t Frequency = PllP;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Dma2d = Ahb1;
	static constexpr uint32_t Eth = Ahb1;
	static constexpr uint32_t Flash = Ahb1;

	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Jpeg = Ahb2;
	static constexpr uint32_t Rng = Ahb2;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;
	static constexpr uint32_t Can3 = Apb1;
	static constexpr uint32_t Cec = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t SpdifRx = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Adc2 = Apb2;
	static constexpr uint32_t Adc3 = Apb2;
	static constexpr uint32_t Adc123Common = Apb2;
	static constexpr uint32_t Dfsdm1 = Apb2;
	static constexpr uint32_t Dsi = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Ltdc = Apb2;
	static constexpr uint32_t Mdios = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;
	static constexpr uint32_t Sdmmc1 = Apb2;
	static constexpr uint32_t Sdmmc2 = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb2;
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
		Rcc::enableHseClock();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);

		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		// Required for 216 MHz operation
		Rcc::enableOverdriveMode();
		Rcc::enableSystemClock(Rcc::SystemClockSource::PllP);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using Button = GpioInputA0;	// User Button
using LedJ13 = GpioOutputJ13;	// User LED 1 (red)
using LedJ5  = GpioOutputJ5;	// User LED 2 (green)
using LedA12 = GpioOutputA12;	// User LED 3 (green)
using LedD4  = GpioInverted<GpioOutputD4>;	// User Led 4 (red)

using Leds = SoftwareGpioPort< LedJ13, LedJ5, LedA12, LedD4 >;
/// @}

namespace stlink
{
/// @ingroup modm_board_disco_f769ni
/// @{
using Tx = GpioOutputA9;
using Rx = GpioInputA10;
using Uart = BufferedUart<UsartHal1, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_disco_f769ni
using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

// Onboard MicroSD card slot
namespace usd
{

// SDIO interface
namespace sdio
{
/// @ingroup modm_board_disco_f769ni
/// @{
using Cmd = GpioOutputD7;
using Clk = GpioOutputD7;
using D3  = GpioB4;
using D2  = GpioB3;
using D1  = GpioG10;
using D0  = GpioG9;
/// @}
}

// SPI interface
namespace spi
{
/// @ingroup modm_board_disco_f769ni
/// @{
using Mosi = sdio::Cmd;
using Clk  = sdio::Clk;
using Cs   = sdio::D3;
using Irq  = sdio::D1;
using Miso = sdio::D0;
/// @}
}
}

/// @ingroup modm_board_disco_f769ni
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	Button::setInput();
}

}

#endif	// MODM_STM32_F769I_DISCOVERY_HPP
