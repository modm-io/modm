/*
 * Copyright (c) 2025, Niklas Hauser
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

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_weact_u585ci
/// @{
using namespace modm::literals;

/// STM32U585 running at 160MHz generated from the external 25MHz crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 25_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll1{
		.range = Rcc::PllInputRange::MHz4_8,
		.M = 5,  //  25 MHz /  5 =   5 MHz
		.N = 96, //   5 MHz * 96 = 480 MHz
		.Q = 10, // 480 MHz / 10 =  48 MHz = F_usb
		.R = 3,  // 480 MHz /  3 = 160 MHz = F_cpu
		.MBoost = Rcc::Pll1MBoost::Div2, // 25 MHz / 2 = 12.5 MHz
	};
	static constexpr uint32_t Pll1R = Hse / pll1.M * pll1.N / pll1.R;
	static constexpr uint32_t Pll1Q = Hse / pll1.M * pll1.N / pll1.Q;
	static_assert(Pll1R == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = Pll1R;
	static constexpr uint32_t Ahb1 = Frequency;
	static constexpr uint32_t Ahb2 = Frequency;
	static constexpr uint32_t Ahb3 = Frequency;

	static constexpr uint32_t Apb1 = Frequency / 1;
	static constexpr uint32_t Apb2 = Frequency / 1;
	static constexpr uint32_t Apb3 = Frequency / 1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Apb3Timer = Apb3 * 2;

	// AHB1 Peripherals
	static constexpr uint32_t Gpdma1 = Ahb1;
	static constexpr uint32_t Cordic = Ahb1;
	static constexpr uint32_t Fmac   = Ahb1;
	static constexpr uint32_t Flash  = Ahb1;
	static constexpr uint32_t Crc    = Ahb1;
	static constexpr uint32_t Tsc    = Ahb1;
	static constexpr uint32_t Mdf1   = Ahb1;
	static constexpr uint32_t Ramcfg = Ahb1;
	static constexpr uint32_t Dma2d  = Ahb1;
	static constexpr uint32_t Icache = Ahb1;
	static constexpr uint32_t Dcache1 = Ahb1;
	static constexpr uint32_t GtzcTzsc1 = Ahb1;
	static constexpr uint32_t GtzcTzic1 = Ahb1;
	static constexpr uint32_t GtzcMpcbb1 = Ahb1;
	static constexpr uint32_t GtzcMpcbb2 = Ahb1;
	static constexpr uint32_t GtzcMpcbb3 = Ahb1;
	static constexpr uint32_t Bkpsram = Ahb1;

	// AHB2 Peripherals
	static constexpr uint32_t Adc1   = Ahb2;
	static constexpr uint32_t Adc12Common = Ahb2;
	static constexpr uint32_t Dcmi   = Ahb2;
	static constexpr uint32_t Pssi   = Ahb2;
	static constexpr uint32_t Dcmipssi = Ahb2;
	static constexpr uint32_t OtgFs  = Ahb2;
	static constexpr uint32_t Aes    = Ahb2;
	static constexpr uint32_t Hash   = Ahb2;
	static constexpr uint32_t HashDigest = Ahb2;
	static constexpr uint32_t Rng    = Ahb2;
	static constexpr uint32_t Saes   = Ahb2;
	static constexpr uint32_t Pka    = Ahb2;
	static constexpr uint32_t PkaRam = Ahb2;
	static constexpr uint32_t OctoSpiM = Ahb2;
	static constexpr uint32_t Otfdec1 = Ahb2;
	static constexpr uint32_t Otfdec2 = Ahb2;
	static constexpr uint32_t Sdmmc1 = Ahb2;
	static constexpr uint32_t Sdmmc2 = Ahb2;
	static constexpr uint32_t DlybSdmmc1 = Ahb2;
	static constexpr uint32_t DlybSdmmc2 = Ahb2;
	static constexpr uint32_t DlybOctoSpi1 = Ahb2;
	static constexpr uint32_t DlybOctoSpi2 = Ahb2;
	static constexpr uint32_t Fmc = Ahb2;
	static constexpr uint32_t OctoSpi1 = Ahb2;
	static constexpr uint32_t OctoSpi2 = Ahb2;

	// AHB3 Peripherals
	static constexpr uint32_t Lpgpio1 = Ahb3;
	static constexpr uint32_t Pwr     = Ahb3;
	static constexpr uint32_t Adc4    = Ahb3;
	static constexpr uint32_t Adc4Common = Ahb3;
	static constexpr uint32_t Dac1    = Ahb3;
	static constexpr uint32_t Exti    = Ahb3;
	static constexpr uint32_t GtzcTzsc2 = Ahb3;
	static constexpr uint32_t GtzcTzic2 = Ahb3;
	static constexpr uint32_t GtzcMpcbb4 = Ahb3;
	static constexpr uint32_t Adf1    = Ahb3;
	static constexpr uint32_t Lpdma1  = Ahb3;

	// APB1 Peripherals
	static constexpr uint32_t Timer2  = Apb1Timer;
	static constexpr uint32_t Timer3  = Apb1Timer;
	static constexpr uint32_t Timer4  = Apb1Timer;
	static constexpr uint32_t Timer5  = Apb1Timer;
	static constexpr uint32_t Timer6  = Apb1Timer;
	static constexpr uint32_t Timer7  = Apb1Timer;
	static constexpr uint32_t Wwdg    = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Spi2    = Apb1;
	static constexpr uint32_t Usart2  = Apb1;
	static constexpr uint32_t Usart3  = Apb1;
	static constexpr uint32_t Uart4   = Apb1;
	static constexpr uint32_t Uart5   = Apb1;
	static constexpr uint32_t I2c1    = Apb1;
	static constexpr uint32_t I2c2    = Apb1;
	static constexpr uint32_t I2c4    = Apb1;
	static constexpr uint32_t Crs     = Apb1;
	static constexpr uint32_t Lptim2  = Apb1;
	static constexpr uint32_t Fdcan1  = Apb1;
	static constexpr uint32_t FdcanConfig = Apb1;
	static constexpr uint32_t SramCan = Apb1;
	static constexpr uint32_t Ucpd1   = Apb1;

	// APB2 Peripherals
	static constexpr uint32_t Timer1  = Apb2Timer;
	static constexpr uint32_t Timer8  = Apb2Timer;
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;
	static constexpr uint32_t Spi1    = Apb2;
	static constexpr uint32_t Usart1  = Apb2;
	static constexpr uint32_t Sai1    = Apb2;
	static constexpr uint32_t Sai2    = Apb2;

	// APB3 Peripherals
	static constexpr uint32_t Syscfg  = Apb3;
	static constexpr uint32_t Spi3    = Apb3;
	static constexpr uint32_t Lpuart1 = Apb3;
	static constexpr uint32_t I2c3    = Apb3;
	static constexpr uint32_t Lptim1  = Apb3;
	static constexpr uint32_t Lptim3  = Apb3;
	static constexpr uint32_t Lptim4  = Apb3;
	static constexpr uint32_t Opamp   = Apb3;
	static constexpr uint32_t Opamp1  = Apb3;
	static constexpr uint32_t Opamp2  = Apb3;
	static constexpr uint32_t Comp    = Apb3;
	static constexpr uint32_t Comp12  = Apb3;
	static constexpr uint32_t Vrefbuf = Apb3;
	static constexpr uint32_t RtcBus  = Apb3;
	static constexpr uint32_t Tamp    = Apb3;

	static constexpr uint32_t Usb = Pll1Q;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range1);

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::enablePll1(Rcc::PllSource::Hse, pll1);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1R);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::Pll1Q);

		return true;
	}
};
/// @}

namespace usb
{
/// @ingroup modm_board_weact_u585ci
/// @{
using Dm = GpioA11;
using Dp = GpioA12;

using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_weact_u585ci
/// @{
using Led = GpioInverted<GpioOutputC13>;
using Leds = SoftwareGpioPort< Led >;

using Button = GpioInverted<GpioInputA0>;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Led::setOutput(modm::Gpio::Low);
	Button::setInput(Gpio::InputType::PullUp);
}

inline void
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}
/// @}

} // Board namespace
