/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Antal Szabó
 * Copyright (c) 2024, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F401_DISCOVERY_HPP
#define MODM_STM32_F401_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/inertial/lsm303a.hpp>
#include <modm/driver/inertial/l3gd20.hpp>

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_disco_f401vc
/// @{
using namespace modm::literals;

/// STM32F401 running at 168MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 84_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Spi1   = Apb2;
	static constexpr uint32_t Spi2   = Apb1;
	static constexpr uint32_t Spi3   = Apb1;
	static constexpr uint32_t Spi4   = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart6 = Apb1;

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

	static constexpr uint32_t Usb = 48_MHz;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal();	// 8MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 4,		// 8MHz / M=4 -> 2MHz
			.pllN = 168,	// 2MHz * N=168 -> 336MHz
			.pllP = 4,		// 336MHz / P=4 ->  84MHz = F_cpu
			.pllQ = 7		// 336MHz / Q=7 ->  48MHz = F_usb
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);
		// set flash latency for 84MHz
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
/// @}

namespace lsm3
{
/// @ingroup modm_board_disco_f401vc
/// @{
using Drdy = GpioInputE2;	// DRDY [LSM303DLHC_DRDY]: GPXTI2
using Int1 = GpioInputE4;	// MEMS_INT3 [LSM303DLHC_INT1]: GPXTI4
using Int2 = GpioInputE5;	// MEMS_INT4 [LSM303DLHC_INT2]: GPXTI5

using Scl = GpioB6;	// I2C1_SCL [LSM303DLHC_SCL]: I2C1_SCL
using Sda = GpioB9;	// I2C1_SDA [LSM303DLHC_SDA]: I2C1_SDA

using I2cMaster = I2cMaster1;
using Accelerometer = modm::Lsm303a< I2cMaster >;
/// @}
}

namespace l3g
{
/// @ingroup modm_board_disco_f401vc
/// @{
using Int1 = GpioInputE0;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
using Int2 = GpioInputE1;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

using Cs   = GpioOutputE3;	// CS_I2C/SPI [L3GD20_CS_I2C/SPI]
using Sck  = GpioOutputA5;	// SPI1_SCK [L3GD20_SCL/SPC]
using Mosi = GpioOutputA7;	// SPI1_MOSI [L3GD20_SDA/SDI/SDO]
using Miso = GpioInputA6;	// SPI1_MISO [L3GD20_SA0/SDO]

using SpiMaster = SpiMaster1;
using Transport = modm::Lis3TransportSpi< SpiMaster, Cs >;
using Gyroscope = modm::L3gd20< Transport >;
/// @}
}

namespace cs43
{
/// @ingroup modm_board_disco_f401vc
/// @{
using Lrck = GpioOutputA4;	// I2S3_WS
using Mclk = GpioOutputC7;	// I2S3_MCK
using Sclk = GpioOutputC10;	// I2S3_SCK
using Sdin = GpioOutputC12;	// I2S3_SD

using Reset = GpioOutputD4;	// Audio_RST
using Scl = GpioB6;			// Audio_SCL
using Sda = GpioB9;			// Audio_SDA

using I2cMaster = I2cMaster1;
//using I2sMaster = I2sMaster3;
/// @}
}


namespace mp45
{
/// @ingroup modm_board_disco_f401vc
/// @{
using Clk = GpioOutputB10;	// CLK_IN: I2S2_CK
using Dout = GpioInputC3;	// PDM_OUT: I2S2_SD
//using I2sMaster = I2sMaster2;
/// @}
}


namespace usb
{
/// @ingroup modm_board_disco_f401vc
/// @{
using Vbus = GpioInputA9;	// VBUS_FS: USB_OTG_HS_VBUS
using Id = GpioA10;			// OTG_FS_ID: USB_OTG_FS_ID
using Dm = GpioA11;			// OTG_FS_DM: USB_OTG_FS_DM
using Dp = GpioA12;			// OTG_FS_DP: USB_OTG_FS_DP

using Overcurrent = GpioInputD5;	// OTG_FS_OverCurrent
using Power = GpioOutputC0;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
/// @}
}


/// @ingroup modm_board_disco_f401vc
/// @{
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Leds::setOutput(modm::Gpio::Low);

	Button::setInput();
}


inline void
initializeLsm3()
{
	lsm3::Int1::setInput();
	lsm3::Int2::setInput();
	lsm3::Drdy::setInput();

	lsm3::I2cMaster::connect<lsm3::Scl::Scl, lsm3::Sda::Sda>();
	lsm3::I2cMaster::initialize<SystemClock, 400_kHz>();
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

inline void
initializeL3g()
{
	l3g::Int1::setInput();
	l3g::Int2::setInput();
	l3g::Cs::setOutput(modm::Gpio::High);

	l3g::SpiMaster::connect<l3g::Sck::Sck, l3g::Mosi::Mosi, l3g::Miso::Miso>();
	l3g::SpiMaster::initialize<SystemClock, 10_MHz>();
	l3g::SpiMaster::setDataMode(l3g::SpiMaster::DataMode::Mode3);
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

#endif	// MODM_STM32_F401_DISCOVERY_HPP
