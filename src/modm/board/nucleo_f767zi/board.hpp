/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_F767ZI_HPP
#define MODM_STM32_NUCLEO_F767ZI_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_nucleo_f767zi
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_f767zi
/// @{
using namespace modm::literals;

/// STM32F7 running at 216MHz from the external 8MHz clock
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 4,   //   8 MHz /   4 =   2 MHz
		.N = 216, //   2 MHz * 216 = 432 MHz
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

// Arduino Footprint
#include "nucleo144_arduino.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputB14;	// LED3 [Red]
using Leds = SoftwareGpioPort< LedRed, LedBlue, LedGreen >;
/// @}

namespace stlink
{
/// @ingroup modm_board_nucleo_f767zi
/// @{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = BufferedUart<UsartHal3, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_f767zi
/// @{
using Sda = D14;
using Scl = D15;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_f767zi
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_f767zi
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
    LedBlue::setOutput(modm::Gpio::Low);
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
	spi::Controller::initialize<SystemClock, 3.375_MHz>();
}
/// @}

}

#endif  // MODM_STM32_NUCLEO_F767ZI_HPP
