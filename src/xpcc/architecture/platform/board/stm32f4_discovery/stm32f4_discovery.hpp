/*
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// STM32F4DISCOVERY
// Discovery kit for STM32F407/417 lines
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF252419
//

#ifndef XPCC_STM32_F4_DISCOVERY_HPP
#define XPCC_STM32_F4_DISCOVERY_HPP

#include <modm/architecture/platform.hpp>
#include <modm/driver/inertial/lis3dsh.hpp>

using namespace xpcc::stm32;


namespace Board
{

/* SystemClock generator is only available for selected STM32F4 devices.
 * The idea is that it is generated automatically for you like the rest of the
 * HAL, however, xpcc does not have this capability yet. See PR #36.
 */
// using systemClock = SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> >;

// Instead this manual implementation of the system clock is used:
/// STM32F407 running at 168MHz generated from the external 8MHz crystal
struct systemClock {
	static constexpr uint32_t Frequency = 168 * MHz1;
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

	static bool inline
	enable()
	{
		ClockControl::enableExternalCrystal();	// 8MHz
		ClockControl::enablePll(
			ClockControl::PllSource::ExternalCrystal,
			4,		// 8MHz / N=2 -> 2MHz
			168,	// 2MHz * M=168 -> 336MHz
			2,		// 336MHz / P=2 -> 168MHz = F_cpu
			7		// 336MHz / Q=7 -> 48MHz = F_usb
		);
		// set flash latency for 168MHz
		ClockControl::setFlashLatency(Frequency);
		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		// APB1 has max. 42MHz
		// APB2 has max. 84MHz
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div4);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div2);
		// update frequencies for busy-wait delay functions
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(3000 / (Frequency / 1000000));

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

using Leds = xpcc::SoftwareGpioPort< LedGreen, LedBlue, LedRed, LedOrange >;


namespace lis3
{
using Int = GpioInputE1;	// LIS302DL_INT2

using Cs   = GpioOutputE3;	// LIS302DL_CS_I2C/SPI
using Sck  = GpioOutputA5;	// SPI1_SCK
using Mosi = GpioOutputA7;	// SPI1_MOSI
using Miso = GpioInputA6;	// SPI1_MISO

using SpiMaster = SpiMaster1;
using Transport = xpcc::Lis3TransportSpi< SpiMaster, Cs >;
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
//using I2sMaster = I2sMaster3;
}


namespace mp45
{
using Clk = GpioOutputB10;	// CLK_IN: I2S2_CK
using Dout = GpioInputC3;	// PDM_OUT: I2S2_SD
//using  = I2sMaster2 I2sMaster;
}


namespace usb
{
using Dm = GpioA11;			// OTG_FS_DM: USB_OTG_FS_DM
using Dp = GpioA12;			// OTG_FS_DP: USB_OTG_FS_DP
using Id = GpioA10;			// OTG_FS_ID: USB_OTG_FS_ID

using Overcurrent = GpioD5;	// OTG_FS_OverCurrent
using Power = GpioOutputC0;	// OTG_FS_PowerSwitchOn
using VBus = GpioInputA9;	// VBUS_FS: USB_OTG_HS_VBUS
//using Device = UsbFs;
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	LedOrange::setOutput(xpcc::Gpio::Low);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::Low);
	LedBlue::setOutput(xpcc::Gpio::Low);

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

	lis3::Cs::setOutput(xpcc::Gpio::High);

	lis3::Sck::connect(lis3::SpiMaster::Sck);
	lis3::Mosi::connect(lis3::SpiMaster::Mosi);
	lis3::Miso::connect(lis3::SpiMaster::Miso);

	lis3::SpiMaster::initialize<systemClock, MHz10>();
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

	cs43::Reset::setOutput(xpcc::Gpio::High);

	cs43::Scl::connect(cs43::I2cMaster::Scl);
	cs43::Sda::connect(cs43::I2cMaster::Sda);
	cs43::I2cMaster::initialize<systemClock, cs43::I2cMaster::Baudrate::Standard>();
}

/// not supported yet, due to missing I2S driver
inline void
initializeMp45()
{
//	mp45::Clk::connect(mp45::I2sMaster::Ck);
//	mp45::Dout::connect(mp45::I2sMaster::Sd);
}

/// not supported yet, due to missing USB driver
inline void
initializeUsb()
{
//	usb::Dm::connect(usb::Device::Dm);
//	usb::Dp::connect(usb::Device::Dp);
//	usb::Id::connect(usb::Device::Id);

	usb::Power::setOutput(Gpio::OutputType::PushPull, Gpio::OutputSpeed::MHz2);

	usb::Overcurrent::setInput(Gpio::InputType::Floating);
	usb::VBus::setInput(Gpio::InputType::Floating);
}

}

#endif	// XPCC_STM32_F4_DISCOVERY_HPP
