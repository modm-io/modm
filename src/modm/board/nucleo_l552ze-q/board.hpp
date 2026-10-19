// coding: utf-8
/*
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_L552ZE_Q_HPP
#define MODM_STM32_NUCLEO_L552ZE_Q_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_nucleo_l552ze_q
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using namespace modm::literals;

/// STM32L5 running at 110 MHz generated from the
/// internal Multispeed oscillator
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 1,  //   4 MHz /  1 =   4 MHz
		.N = 55, //   4 MHz * 55 = 220 MHz
		.R = 2,  // 220 MHz /  2 = 110 MHz = F_cpu
		.Q = 2,  // 220 MHz /  2 = 110 MHz
	};
	static constexpr uint32_t PllR = Rcc::MsiFrequency / pll.M * pll.N / pll.R;
	static_assert(PllR == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = PllR;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Ahb3 = Ahb;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;
	static constexpr uint32_t Clk48 = 48_MHz;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Dmamux1 = Ahb1;
	static constexpr uint32_t Exti = Ahb1;
	static constexpr uint32_t Flash = Ahb1;
	static constexpr uint32_t Icache = Ahb1;
	static constexpr uint32_t Tsc = Ahb1;

	static constexpr uint32_t Adc1 = Ahb2;
	static constexpr uint32_t Adc2 = Ahb2;
	static constexpr uint32_t Adc12Common = Ahb2;
	static constexpr uint32_t Hash = Ahb2;
	static constexpr uint32_t Rng = Clk48;
	static constexpr uint32_t Sdmmc1 = Clk48;

	static constexpr uint32_t Fmc = Ahb3;
	static constexpr uint32_t Octospi1 = Ahb3;

	static constexpr uint32_t Crs = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t Dac1 = Apb1;
	static constexpr uint32_t Fdcan1 = Apb1;
	static constexpr uint32_t FdcanConfig = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Lptim2 = Apb1;
	static constexpr uint32_t Lptim3 = Apb1;
	static constexpr uint32_t Lpuart1 = Apb1;
	static constexpr uint32_t Opamp = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Tamp = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Ucpd1 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Usb = Clk48;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Comp1 = Apb2;
	static constexpr uint32_t Comp2 = Apb2;
	static constexpr uint32_t Dfsdm1 = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Vrefbuf = Apb2;

	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHsi48Clock();

		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range0);
		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::enablePll(Rcc::PllSource::Msi, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setClk48Source(Rcc::Clk48Source::Hsi48);

		return true;
	}
};

// Arduino Footprint
#include "nucleo144_arduino_l5.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputC7;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputA9;	// LED3 [Red]
using Leds = SoftwareGpioPort<LedRed, LedBlue, LedGreen>;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using UcpdFlt = GpioB14;
using UcpdDBn = GpioB5;
using UcpdCc1 = GpioA15;
using UcpdCc2 = GpioB15;

using Device = UsbFs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using Rx = GpioOutputG8;
using Tx = GpioInputG7;
using Uart = BufferedUart<LpuartHal1, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using Sda = D14;
using Scl = D15;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_l552ze_q
/// @{
using LoggerDevice = modm::IODeviceWrapper<stlink::Uart, modm::IOBuffer::BlockIfFull>;

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
	spi::Controller::initialize<SystemClock, 3.4375_MHz>();
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}

/// @}

} // Board namespace

#endif	// MODM_STM32_NUCLEO_L552ZE_Q_HPP
