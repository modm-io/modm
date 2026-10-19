/*
 * Copyright (c) 2021, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_H750VB_DEVEBOX_HPP
#define MODM_STM32_H750VB_DEVEBOX_HPP

#include <modm/platform.hpp>
#include <modm/architecture.hpp>
#include <modm/debug.hpp>

using namespace modm::platform;

/// @ingroup modm_board_devebox_stm32h750vb
#define MODM_BOARD_HAS_LOGGER

namespace Board
{
/// @ingroup modm_board_devebox_stm32h750vb
/// @{
using namespace modm::literals;

/// STM32H750 running at 400MHz from the external 25MHz HSE
struct SystemClock
{
	static constexpr uint32_t Hse = 25_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll1
	{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 2,   //   25 MHz /  2 = 12.5 MHz
		.N = 64,  // 12.5 MHz * 64 =  800 MHz
		.P = 2,   //  800 MHz /  2 =  400 MHz = F_cpu
		.Q = 20,  //  800 MHz / 20 =   40 MHz
		.R = 20,  //  800 MHz / 20 =   40 MHz
	};
	static constexpr Rcc::PllConfig pll3
	{
		.range = Rcc::PllInputRange::MHz4_8,
		.M = 5,   //   25 MHz /  5 =    5 MHz
		.N = 48,  //    5 MHz * 48 =  240 MHz
		.P = 2,   //  240 MHz /  2 =  120 MHz
		.Q = 5,   //  240 MHz /  5 =   48 MHz = F_usb
		.R = 5,   //  240 MHz /  5 =   48 MHz
	};
	static constexpr uint32_t Pll1P = Hse / pll1.M * pll1.N / pll1.P;
	static constexpr uint32_t Pll1Q = Hse / pll1.M * pll1.N / pll1.Q;
	static constexpr uint32_t Pll3Q = Hse / pll3.M * pll3.N / pll3.Q;
	// NOTE: revision Y runs at 400MHz, only revision V runs at 480MHz!
	// Some of the devebox boards including the original one have the Y revision!
	static_assert(Pll1P == 400_MHz);
	static_assert(Pll3Q == 48_MHz);

	static constexpr uint32_t SysClk = Pll1P;
	static constexpr uint32_t Frequency = SysClk;
	static constexpr uint32_t Ahb = Frequency / 2;
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
	static constexpr uint32_t Eth = Ahb1;

	static constexpr uint32_t Cryp = Ahb2;
	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Hash = Ahb2;
	static constexpr uint32_t Rng = Ahb2;
	static constexpr uint32_t Sdmmc2 = Pll1Q;

	static constexpr uint32_t Dma2d = Ahb3;
	static constexpr uint32_t Flash = Ahb3;
	static constexpr uint32_t Fmc = Ahb3;
	static constexpr uint32_t Jpgdec = Ahb3;
	static constexpr uint32_t Mdma = Ahb3;
	static constexpr uint32_t Qspi = Ahb3;
	static constexpr uint32_t Sdmmc1 = Pll1Q;

	static constexpr uint32_t Adc3 = Ahb4;
	static constexpr uint32_t Adc3Common = Ahb4;
	static constexpr uint32_t Bdma = Ahb4;
	static constexpr uint32_t Crc = Ahb4;
	static constexpr uint32_t Dmamux2 = Ahb4;
	static constexpr uint32_t Hsem = Ahb4;
	static constexpr uint32_t Pwr = Ahb4;

	static constexpr uint32_t Cec = Apb1;
	static constexpr uint32_t Crs = Apb1;
	static constexpr uint32_t Dac1 = Apb1;
	static constexpr uint32_t Fdcan1 = Hse;
	static constexpr uint32_t Fdcan2 = Hse;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t Lptim1 = Apb1;
	static constexpr uint32_t Mdios = Apb1;
	static constexpr uint32_t Opamp = Apb1;
	static constexpr uint32_t SpdifRx = Apb1;
	static constexpr uint32_t Spi2 = Pll1Q;
	static constexpr uint32_t Spi3 = Pll1Q;
	static constexpr uint32_t Swpmi1 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;

	static constexpr uint32_t Dfsdm1 = Apb2;
	static constexpr uint32_t Hrtim1 = Apb2;
	static constexpr uint32_t Sai1 = Apb2;
	static constexpr uint32_t Sai2 = Apb2;
	static constexpr uint32_t Sai3 = Apb2;
	static constexpr uint32_t Spi1 = Pll1Q;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart6 = Apb2;

	static constexpr uint32_t Ltdc = Apb3;
	static constexpr uint32_t Wwdg1 = Apb3;

	static constexpr uint32_t Comp12 = Apb4;
	static constexpr uint32_t Exti = Apb4;
	static constexpr uint32_t I2c4 = Apb4;
	static constexpr uint32_t IwdgBus = Apb4;
	static constexpr uint32_t Lpuart1 = Apb4;
	static constexpr uint32_t Lptim2 = Apb4;
	static constexpr uint32_t Lptim3 = Apb4;
	static constexpr uint32_t Lptim4 = Apb4;
	static constexpr uint32_t Lptim5 = Apb4;
	static constexpr uint32_t RtcBus = Apb4;
	static constexpr uint32_t Sai4 = Apb4;
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

	static constexpr uint32_t Usb = Pll3Q;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		// Required for running at 400 MHz
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Scale0);
		Rcc::setFlashLatency<Ahb>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setCpuPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div2);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div2);
		Rcc::setApb4Prescaler(Rcc::ApbPrescaler::Div2);

		Rcc::enablePll1(Rcc::PllSource::Hse, pll1);
		Rcc::enablePll3(Rcc::PllSource::Hse, pll3);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1P);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::Pll3Q);

		return true;
	}
};

using ButtonK1 = GpioInverted<GpioInputE3>;
using ButtonK2 = GpioInverted<GpioInputC5>;
using Button = ButtonK1;

using LedGreen = GpioInverted<GpioOutputA1>;	// User LED

using Leds = SoftwareGpioPort< LedGreen >;

using LoggerDevice = modm::IODeviceWrapper< Rtt<0>, modm::IOBuffer::DiscardIfFull >;
/// @}

namespace usb
{
/// @ingroup modm_board_devebox_stm32h750vb
/// @{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
/// @}
}

/// @ingroup modm_board_devebox_stm32h750vb
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen::setOutput(modm::Gpio::Low);
	ButtonK1::setInput(Gpio::InputType::PullUp);
	ButtonK2::setInput(Gpio::InputType::PullUp);
}

}

#endif	// MODM_STM32_H750VB_DEVEBOX_HPP
