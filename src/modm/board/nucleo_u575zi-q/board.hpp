// coding: utf-8
/*
 * Copyright (c) 2022, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_NUCLEO_U575ZI_Q_HPP
#define MODM_STM32_NUCLEO_U575ZI_Q_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_nucleo_u575zi_q
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_nucleo_u575zi_q
/// @{
using namespace modm::literals;

/// STM32U5 running at 160 MHz generated from the internal HSI16 oscillator
struct SystemClock
{
	static constexpr uint32_t Lse = 32.768_kHz;

	static constexpr Rcc::PllConfig pll1{
		.range = Rcc::PllInputRange::MHz8_16,
		.M = 2,  //  16 MHz /  2 = 8 MHz
		.N = 60, //   8 MHz * 60 = 480 MHz
		.Q = 10, // 480 MHz / 10 =  48 MHz = F_usb
		.R = 3,  // 480 MHz /  3 = 160 MHz = F_cpu
	};
	static constexpr uint32_t Pll1R = Rcc::HsiFrequency / pll1.M * pll1.N / pll1.R;
	static constexpr uint32_t Pll1Q = Rcc::HsiFrequency / pll1.M * pll1.N / pll1.Q;
	static_assert(Pll1R == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = Pll1R;
	static constexpr uint32_t Ahb  = Frequency;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;
	static constexpr uint32_t Ahb3 = Ahb;
	static constexpr uint32_t Apb1 = Frequency;
	static constexpr uint32_t Apb2 = Frequency;
	static constexpr uint32_t Apb3 = Frequency;
	static constexpr uint32_t Apb1Timer = Apb1;
	static constexpr uint32_t Apb2Timer = Apb2;

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
	static constexpr uint32_t GpioA  = Ahb2;
	static constexpr uint32_t GpioB  = Ahb2;
	static constexpr uint32_t GpioC  = Ahb2;
	static constexpr uint32_t GpioD  = Ahb2;
	static constexpr uint32_t GpioE  = Ahb2;
	static constexpr uint32_t GpioF  = Ahb2;
	static constexpr uint32_t GpioG  = Ahb2;
	static constexpr uint32_t GpioH  = Ahb2;
	static constexpr uint32_t GpioI  = Ahb2;
	static constexpr uint32_t Adc1   = Ahb2;
	static constexpr uint32_t Adc12Common = Ahb2;
	static constexpr uint32_t Dcmi   = Ahb2;
	static constexpr uint32_t Pssi   = Ahb2;
	static constexpr uint32_t OtgFs  = Ahb2;
	static constexpr uint32_t Hash   = Ahb2;
	static constexpr uint32_t HashDigest = Ahb2;
	static constexpr uint32_t Rng    = Ahb2;
	static constexpr uint32_t OctoSpiM = Ahb2;
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
		Rcc::enableHsiClock();
		Rcc::setVoltageScaling(Rcc::VoltageScaling::Range1);

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div1);
		Rcc::setApb3Prescaler(Rcc::ApbPrescaler::Div1);

		Rcc::enablePll1(Rcc::PllSource::Hsi, pll1);
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll1R);

		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);
		Rcc::setUsbClockSource(Rcc::UsbClockSource::Pll1Q);

		return true;
	}
};

// Arduino Footprint
#include "nucleo144_arduino_u5.hpp"

using Button = GpioInputC13;

using LedGreen = GpioOutputC7;	// LED1 [Green]
using LedBlue = GpioOutputB7;	// LED2 [Blue]
using LedRed = GpioOutputG2;	// LED3 [Red]
using Leds = SoftwareGpioPort<LedRed, LedBlue, LedGreen>;
/// @}

namespace usb
{
/// @ingroup modm_board_nucleo_u575zi_q
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
/// @ingroup modm_board_nucleo_u575zi_q
/// @{
using Rx = GpioOutputA10;
using Tx = GpioInputA9;
using Uart = BufferedUart<UsartHal1, UartTxBuffer<2048>>;
/// @}
}

/// @ingroup modm_board_nucleo_u575zi_q
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
initializeUsb(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}

/// @}

} // Board namespace

#endif	// MODM_STM32_NUCLEO_U575ZI_Q_HPP
