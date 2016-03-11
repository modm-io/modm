// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F4DISCOVERY
// Discovery kit for STM32F407/417 lines
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF252419
//

#ifndef XPCC_STM32_F4_DISCOVERY_HPP
#define XPCC_STM32_F4_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/inertial/lis3dsh.hpp>

using namespace xpcc::stm32;


namespace Board
{

/// STM32F4 running at 168MHz (USB Clock qt 48MHz) generated from the
/// external on-board 8MHz crystal
using systemClock = SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> >;


using Button = GpioInputA0;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using LedOrange = GpioOutputD13;	// User LED 3
using LedGreen  = GpioOutputD12;	// User LED 4
using LedRed    = GpioOutputD14;	// User LED 5
using LedBlue   = GpioOutputD15;	// User LED 6



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
