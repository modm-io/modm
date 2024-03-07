/*
 * Copyright (c) 2024, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/inertial/lis3dsh.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_disco_f411ve
/// @{
using namespace modm::literals;

/// STM32F411 running at 96MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 96_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7 = Apb1;
	static constexpr uint32_t Uart8 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;

	static constexpr uint32_t Apb1Timer = Apb1 * 2;
	static constexpr uint32_t Apb2Timer = Apb2 * 2;
	static constexpr uint32_t Timer1 = Apb2Timer;
	static constexpr uint32_t Timer2 = Apb1Timer;
	static constexpr uint32_t Timer3 = Apb1Timer;
	static constexpr uint32_t Timer4 = Apb1Timer;
	static constexpr uint32_t Timer5 = Apb1Timer;
	static constexpr uint32_t Timer9 = Apb2Timer;
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;

	static constexpr uint32_t Usb = 48_MHz;

	static bool inline enable()
	{
		Rcc::enableExternalCrystal();  // 8MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 7,    // 8MHz / M=7 -> ~1.14MHz
			.pllN = 336,  // 1.14MHz * N=336 -> 384MHz
			.pllP = 4,    // 384MHz / P=4 -> 96MHz = F_cpu
			.pllQ = 8,    // 384MHz / P=8 -> 48MHz = F_usb
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);
		// set flash latency for 100MHz
		Rcc::setFlashLatency<Frequency>();
		// switch system clock to PLL output
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		// APB1 has max. 50MHz
		// APB2 has max. 100MHz
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div2);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};

using LedUsb = GpioOutputA9;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using Button = GpioInputA0;

// 4 user colored LEDs aligned in a circle
using LedGreen = GpioOutputD12;
using LedOrange = GpioOutputD13;
using LedRed = GpioOutputD14;
using LedBlue = GpioOutputD15;
using Leds = SoftwareGpioPort<LedGreen, LedBlue, LedRed, LedOrange>;
/// @}

namespace lis3
{
/// @ingroup modm_board_disco_f411ve
/// @{
using Int = GpioInputE1;  // LIS302DL_INT2

using Cs = GpioOutputE3;    // LIS302DL_CS_I2C/SPI
using Sck = GpioOutputA5;   // SPI1_SCK
using Mosi = GpioOutputA7;  // SPI1_MOSI
using Miso = GpioInputA6;   // SPI1_MISO

using SpiMaster = SpiMaster1;
using Transport = modm::Lis3TransportSpi<SpiMaster, Cs>;
/// @}
}


namespace cs43
{
/// @ingroup modm_board_disco_f411ve
/// @{
using Lrck = GpioOutputA4;   // I2S3_WS
using Mclk = GpioOutputC7;   // I2S3_MCK
using Sclk = GpioOutputC10;  // I2S3_SCK
using Sdin = GpioOutputC12;  // I2S3_SD

using Reset = GpioOutputD4;  // Audio_RST
using Scl = GpioB6;          // Audio_SCL
using Sda = GpioB9;          // Audio_SDA

using I2cMaster = I2cMaster1;
// using I2sMaster = I2sMaster3;
/// @}
}


namespace mp45
{
/// @ingroup modm_board_disco_f411ve
/// @{
using Clk = GpioOutputB10;  // CLK_IN: I2S2_CK
using Dout = GpioInputC3;   // PDM_OUT: I2S2_SD
// using I2sMaster = I2sMaster2;
/// @}
}


namespace usb
{
/// @ingroup modm_board_disco_f411ve
/// @{
using Vbus = GpioInputA9;  // VBUS_FS: USB_OTG_HS_VBUS
using Id = GpioA10;        // OTG_FS_ID: USB_OTG_FS_ID
using Dm = GpioA11;        // OTG_FS_DM: USB_OTG_FS_DM
using Dp = GpioA12;        // OTG_FS_DP: USB_OTG_FS_DP

using Overcurrent = GpioInputD5;  // OTG_FS_OverCurrent
using Power = GpioOutputC0;       // OTG_FS_PowerSwitchOn

using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_disco_f411ve
/// @{
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Leds::setOutput(modm::Gpio::Low);
	LedUsb::setOutput(modm::Gpio::Low);

	Button::setInput(Gpio::InputType::Floating);
}

inline void
initializeLis3()
{
	lis3::Int::setInput();
	lis3::Cs::setOutput(modm::Gpio::High);

	lis3::SpiMaster::connect<lis3::Sck::Sck, lis3::Mosi::Mosi, lis3::Miso::Miso>();
	lis3::SpiMaster::initialize<SystemClock, 6_MHz>();
	lis3::SpiMaster::setDataMode(lis3::SpiMaster::DataMode::Mode3);
}

/// not supported yet, due to missing I2S driver
inline void
initializeCs43()
{
	//	cs43::Lrck::connect(cs43::I2sMaster::Ws);
	//	cs43::Mclk::connect(cs43::I2sMaster::Mck);
	//	cs43::Sclk::connect(cs43::I2sMaster::Ck);
	//	cs43::Sdin::connect(cs43::I2sMaster::Sd);

	cs43::Reset::setOutput(modm::Gpio::High);

	cs43::I2cMaster::connect<cs43::Scl::Scl, cs43::Sda::Sda>();
	cs43::I2cMaster::initialize<SystemClock, 100_kHz>();
}

/// not supported yet, due to missing I2S driver
inline void
initializeMp45()
{
	//	mp45::Clk::connect(mp45::I2sMaster::Ck);
	//	mp45::Dout::connect(mp45::I2sMaster::Sd);
}

inline void
initializeUsbFs(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG &= ~USB_OTG_GCCFG_NOVBUSSENS;
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBUSBSEN;
}
/// @}
}
