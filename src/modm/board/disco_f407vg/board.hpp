/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F4_DISCOVERY_HPP
#define MODM_STM32_F4_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/inertial/lis3dsh.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_f407vg
namespace Board
{
	using namespace modm::literals;

/// STM32F407 running at 168MHz generated from the external 8MHz crystal
struct SystemClock {
	static constexpr uint32_t Frequency = 168_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Can1   = Apb1;
	static constexpr uint32_t Can2   = Apb1;

	static constexpr uint32_t Spi1   = Apb2;
	static constexpr uint32_t Spi2   = Apb1;
	static constexpr uint32_t Spi3   = Apb1;
	static constexpr uint32_t Spi4   = Apb2;
	static constexpr uint32_t Spi5   = Apb2;
	static constexpr uint32_t Spi6   = Apb2;

	static constexpr uint32_t I2s2   = Spi2;
	static constexpr uint32_t I2s3   = Spi3;

	static constexpr uint32_t I2sPll = 86_MHz;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t I2c1   = Apb1;
	static constexpr uint32_t I2c2   = Apb1;
	static constexpr uint32_t I2c3   = Apb1;

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

	static constexpr uint32_t Usb = 48_MHz;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();	// 8MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 8,		// 8MHz / M=8 -> 1MHz
			.pllN = 336,	// 1MHz * N=336 -> 336MHz
			.pllP = 2,		// 336MHz / P=2 -> 168MHz = F_cpu
			.pllQ = 7		// 336MHz / Q=7 ->  48MHz = F_usb
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);
		const Rcc::PllI2sFactors pllI2sFactors{
			.pllN = 258,	// 1 MHz * N=258 -> 258 MHz
			.pllR = 3		// 258 MHz / R=3 -> 86 MHz
		};
		Rcc::enablePllI2s(pllI2sFactors, 2048);
		// set flash latency for 168MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 42MHz
		// APB2 has max. 84MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};


using Button = GpioInputA0;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using LedOrange = GpioOutputD13;	// User LED 3
using LedGreen  = GpioOutputD12;	// User LED 4
using LedRed    = GpioOutputD14;	// User LED 5
using LedBlue   = GpioOutputD15;	// User LED 6

using Leds = SoftwareGpioPort< LedGreen, LedBlue, LedRed, LedOrange >;


namespace lis3
{
using Int = GpioInputE1;	// LIS302DL_INT2

using Cs   = GpioOutputE3;	// LIS302DL_CS_I2C/SPI
using Sck  = GpioOutputA5;	// SPI1_SCK
using Mosi = GpioOutputA7;	// SPI1_MOSI
using Miso = GpioInputA6;	// SPI1_MISO

using SpiMaster = SpiMaster1;
using Transport = modm::Lis3TransportSpi< SpiMaster, Cs >;
}


namespace cs43
{
using Lrck = GpioOutputA4;	// I2S3_WS
using Mclk = GpioOutputC7;	// I2S3_MCK
using Sclk = GpioOutputC10;	// I2S3_SCK
using Sdin = GpioOutputC12;	// I2S3_SD

using Reset = GpioOutputD4;	// Audio_RST
using Scl = GpioB6;			// Audio_SCL
using Sda = GpioB9;			// Audio_SDA

using I2cMaster = I2cMaster1;
using I2sMaster = I2sMaster3;
}


namespace mp45
{
using Clk = GpioOutputB10;	// CLK_IN: I2S2_CK
using Dout = GpioInputC3;	// PDM_OUT: I2S2_SD
//using I2sMaster = I2sMaster2;
}


namespace usb
{
using Vbus = GpioInputA9;	// VBUS_FS: USB_OTG_HS_VBUS
using Id = GpioA10;			// OTG_FS_ID: USB_OTG_FS_ID
using Dm = GpioA11;			// OTG_FS_DM: USB_OTG_FS_DM
using Dp = GpioA12;			// OTG_FS_DP: USB_OTG_FS_DP

using Overcurrent = GpioInputD5;	// OTG_FS_OverCurrent
using Power = GpioOutputC0;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
}


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Leds::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

inline void
initializeLis3()
{
	lis3::Int::setInput();
	lis3::Int::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	lis3::Int::enableExternalInterrupt();
//	lis3::Int::enableExternalInterruptVector(12);

	lis3::Cs::setOutput(modm::Gpio::High);

	lis3::SpiMaster::connect<lis3::Sck::Sck, lis3::Mosi::Mosi, lis3::Miso::Miso>();
	lis3::SpiMaster::initialize<SystemClock, 10_MHz>();
	lis3::SpiMaster::setDataMode(lis3::SpiMaster::DataMode::Mode3);
}

/// not supported yet, due to missing I2S driver
inline void
initializeCs43()
{
	cs43::I2sMaster::connect<cs43::Mclk::Mck, cs43::Sclk::Ck,
							cs43::Lrck::Ws, cs43::Sdin::Sd>();
	cs43::I2sMaster::initialize<SystemClock, 48_kHz>();
	cs43::Reset::setOutput(modm::Gpio::High);

	cs43::I2cMaster::connect<cs43::Scl::Scl, cs43::Sda::Sda>();
	cs43::I2cMaster::initialize<SystemClock, 100_kHz>();

	cs43::Reset::setOutput(modm::Gpio::Low);
	modm::delay_ms(2);
	cs43::Reset::setOutput(modm::Gpio::High);
	
}

/// not supported yet, due to missing I2S driver
inline void
initializeMp45()
{
//	mp45::Clk::connect(mp45::I2sMaster::Ck);
//	mp45::Dout::connect(mp45::I2sMaster::Sd);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
}

}

#endif	// MODM_STM32_F4_DISCOVERY_HPP
