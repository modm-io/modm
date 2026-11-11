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
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

using namespace modm::platform;

/// @ingroup modm_board_nucleo_h7a3zi_q
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_nucleo_h7a3zi_q
/// @{
using namespace modm::literals;

/// STM32H7A3ZI running at 280MHz from PLL clock generated from the 8 MHz ST-LINK MCO
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll1
	{
		.range = Rcc::PllInputRange::MHz4_8,
		.M = 2,   //   8 MHz /   2 =   4 MHz
		.N = 140, //   4 MHz * 140 = 560 MHz
		.P = 2,   // 560 MHz /   2 = 280 MHz = F_cpu
		.Q = 7,   // 560 MHz /   7 =  80 MHz = F_spi, F_fdcan
		.R = 2,   // 560 MHz /   2 = 280 MHz
	};
	static constexpr uint32_t Pll1P = Hse / pll1.M * pll1.N / pll1.P;
	static constexpr uint32_t Pll1Q = Hse / pll1.M * pll1.N / pll1.Q;
	static_assert(Pll1P == Rcc::MaxFrequency);

	static constexpr uint32_t SysClk = Pll1P;
	static constexpr uint32_t Frequency = SysClk;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Ahb3 = Ahb;
	static constexpr uint32_t Ahb4 = Ahb;
	static constexpr uint32_t Apb1 = Ahb / 2;
	static constexpr uint32_t Apb2 = Ahb / 2;
	static constexpr uint32_t Apb3 = Ahb / 2;
	static constexpr uint32_t Apb4 = Ahb / 2;

	static constexpr uint32_t Adc1 = Ahb1;
	static constexpr uint32_t Adc2 = Ahb1;
	static constexpr uint32_t Adc12Common = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Dmamux1 = Ahb1;

	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Pssi = Ahb2;
	static constexpr uint32_t Rng = Ahb2;
	static constexpr uint32_t Sdmmc2 = Pll1Q;

	static constexpr uint32_t Dma2d = Ahb3;
	static constexpr uint32_t Flash = Ahb3;
	static constexpr uint32_t Fmc = Ahb3;
	static constexpr uint32_t Mdma = Ahb3;
	static constexpr uint32_t Octospi1 = Ahb3;
	static constexpr uint32_t Octospi2 = Ahb3;
	static constexpr uint32_t Sdmmc1 = Pll1Q;

	static constexpr uint32_t Bdma = Ahb4;
	static constexpr uint32_t Crc = Ahb4;
	static constexpr uint32_t Dmamux2 = Ahb4;
	static constexpr uint32_t Hsem = Ahb4;
	static constexpr uint32_t Pwr = Ahb4;

	static constexpr uint32_t Cec = Apb1;
	static constexpr uint32_t Crs = Apb1;
	static constexpr uint32_t Dac1 = Apb1;
	static constexpr uint32_t Fdcan1 = Pll1Q;
	static constexpr uint32_t Fdcan2 = Pll1Q;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Opamp = Apb1;
	static constexpr uint32_t SpdifRx = Apb1;
	static constexpr uint32_t Spi2 = Pll1Q;
	static constexpr uint32_t Spi3 = Pll1Q;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;

	static constexpr uint32_t Dfsdm1 = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;
	static constexpr uint32_t Spi1 = Pll1Q;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart6 = Apb2;

	static constexpr uint32_t Ltdc = Apb3;
	static constexpr uint32_t Wwdg1 = Apb3;

	static constexpr uint32_t Comp12 = Apb4;
	static constexpr uint32_t Dfsdm2 = Apb4;
	static constexpr uint32_t Dts = Apb4;
	static constexpr uint32_t Exti = Apb4;
	static constexpr uint32_t I2c4 = Apb4;
	static constexpr uint32_t IwdgBus = Apb4;
	static constexpr uint32_t Lpuart1 = Apb4;
	static constexpr uint32_t Lptim2 = Apb4;
	static constexpr uint32_t Lptim3 = Apb4;
	static constexpr uint32_t Lptim4 = Apb4;
	static constexpr uint32_t Lptim5 = Apb4;
	static constexpr uint32_t RtcBus = Apb4;
	static constexpr uint32_t Spi6 = Apb4;
	static constexpr uint32_t Syscfg = Apb4;
	static constexpr uint32_t Vrefbuf = Apb4;

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
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static constexpr uint32_t Usb = 48_MHz; // HSI48
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseClock(); // ST-LINK MCO
		Rcc::enableHsi48Clock();

		// Required for running at 280 MHz
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0);
		Rcc::setFlashLatency<Ahb>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setCpuPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb4Prescaler(Rcc::ApbPrescaler::Div2);

		if (not Rcc::enablePll1(Rcc::PllSource::Hse, pll1)) return false;
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setCanClockSource(Rcc::CanClockSource::Pll1Q);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::Hsi48);

		return true;
	}
};

// Arduino Uno footprint of the NUCLEO-H7A3ZI-Q (UM2408 Table 18)
using A0 = GpioA3;
using A1 = GpioC0;
using A2 = GpioC3;
using A3 = GpioB1;
using A4 = GpioC2;
using A5 = GpioF11;
using A6 = GpioC1;
using A7 = GpioC5;
using A8 = GpioA2;

using D0  = GpioB7;
using D1  = GpioB6;
using D2  = GpioG14;
using D3  = GpioE13;
using D4  = GpioE14;
using D5  = GpioE11;
using D6  = GpioA8;
using D7  = GpioG12;
using D8  = GpioG9;
using D9  = GpioD15;
using D10 = GpioD14;
using D11 = GpioA7;
using D12 = GpioA6;
using D13 = GpioA5;
using D14 = GpioB9;
using D15 = GpioB8;

using Button = GpioInputC13;

using LedGreen = GpioOutputB0;
using LedYellow = GpioOutputE1;
using LedRed = GpioOutputB14;
using Leds = SoftwareGpioPort< LedRed, LedYellow, LedGreen >;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_h7a3zi_q
/// @{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputG7;
using Power = GpioOutputF10;

using Device = UsbHs;
/// @}
}

namespace stlink
{
/// @ingroup modm_board_nucleo_h7a3zi_q
/// @{
using Tx = GpioOutputD8;
using Rx = GpioInputD9;
using Uart = BufferedUart<UsartHal3, UartTxBuffer<2048>>;
/// @}
}

namespace i2c
{
/// @ingroup modm_board_nucleo_h7a3zi_q
/// @{
using Sda = D14;
using Scl = D15;
using Controller = I2cMaster1;
/// @}
}

namespace spi
{
/// @ingroup modm_board_nucleo_h7a3zi_q
/// @{
using Cs = D10;
using Sck = D13;
using Sdi = D12;
using Sdo = D11;
using Controller = SpiMaster1;
/// @}
}

/// @ingroup modm_board_nucleo_h7a3zi_q
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
	LedYellow::setOutput(modm::Gpio::Low);
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
	spi::Controller::initialize<SystemClock, 5_MHz>();
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_HS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}

/// @}

}
