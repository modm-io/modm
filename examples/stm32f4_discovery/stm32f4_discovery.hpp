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
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> > systemClock;


typedef GpioInputA0  Button;	// Blue PushButton
typedef GpioOutputA8 ClockOut;
typedef GpioOutputC9 SystemClockOut;


typedef GpioOutputD13 LedOrange;	// User LED 3
typedef GpioOutputD12 LedGreen;		// User LED 4
typedef GpioOutputD14 LedRed;		// User LED 5
typedef GpioOutputD15 LedBlue;		// User LED 6


namespace lis3
{
typedef GpioInputE1		Int;	// LIS302DL_INT2

typedef GpioOutputE3	Cs;		// LIS302DL_CS_I2C/SPI
typedef GpioOutputA5	Sck;	// SPI1_SCK
typedef GpioOutputA7	Mosi;	// SPI1_MOSI
typedef GpioInputA6		Miso;	// SPI1_MISO

typedef SpiMaster1 SpiMaster;
typedef xpcc::Lis3TransportSpi< SpiMaster, Cs > Transport;
}


namespace cs43
{
typedef GpioOutputA4	Lrck;	// I2S3_WS
typedef GpioOutputC7	Mclk;	// I2S3_MCK
typedef GpioOutputC10	Sclk;	// I2S3_SCK
typedef GpioOutputC12	Sdin;	// I2S3_SD

typedef GpioOutputD4	Reset;	// Audio_RST
typedef GpioB6			Scl;	// Audio_SCL
typedef GpioB9			Sda;	// Audio_SDA

typedef I2cMaster1 I2cMaster;
//typedef I2sMaster3 I2sMaster;
}


namespace mp45
{
typedef GpioOutputB10	Clk;	// CLK_IN: I2S2_CK
typedef GpioInputC3		Dout;	// PDM_OUT: I2S2_SD
//typedef I2sMaster2 I2sMaster;
}


namespace usb
{
typedef GpioA11	Dm;		// OTG_FS_DM: USB_OTG_FS_DM
typedef GpioA12	Dp;		// OTG_FS_DP: USB_OTG_FS_DP
typedef GpioA10	Id;		// OTG_FS_ID: USB_OTG_FS_ID

typedef GpioD5			Overcurrent;	// OTG_FS_OverCurrent
typedef GpioOutputC0	Power;			// OTG_FS_PowerSwitchOn
typedef GpioInputA9		VBus;			// VBUS_FS: USB_OTG_HS_VBUS
//typedef UsbFs Device;
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

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
