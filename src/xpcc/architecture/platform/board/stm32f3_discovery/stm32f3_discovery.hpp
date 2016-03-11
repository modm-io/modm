// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F3DISCOVERY
// Discovery kit for STM32 F3 series
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF254044
//

#ifndef XPCC_STM32_F3_DISCOVERY_HPP
#define XPCC_STM32_F3_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/inertial/l3gd20.hpp>
#include <xpcc/driver/inertial/lsm303a.hpp>

using namespace xpcc::stm32;


namespace Board
{

/// STM32F3 running at 72MHz generated from the external 8MHz clock
/// supplied by the on-board st-link
using systemClock = SystemClock<Pll<ExternalClock<MHz8>, MHz72> >;


using Button = GpioInputA0;
using ClockOut = GpioOutputA8;

using LedNorth     = GpioOutputE9;		// User LED 3: Red
using LedNorthEast = GpioOutputE10;		// User LED 5: Orange
using LedEast      = GpioOutputE11;		// User LED 7: Green
using LedSouthEast = GpioOutputE12;		// User LED 9: Blue
using LedSouth     = GpioOutputE13 ;	// User LED 10: Red
using LedSouthWest = GpioOutputE14;		// User LED 8: Orange
using LedWest      = GpioOutputE15;		// User LED 6: Green
using LedNorthWest = GpioOutputE8;		// User LED 4: Blue



namespace l3g
{
using Int1 = GpioInputE0;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
using Int2 = GpioInputE1;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

using Cs   = GpioOutputE3;	// CS_I2C/SPI [L3GD20_CS_I2C/SPI]
using Sck  = GpioOutputA5;	// SPI1_SCK [L3GD20_SCL/SPC]
using Mosi = GpioOutputA7;	// SPI1_MISO [L3GD20_SDA/SDI/SDO]
using Miso = GpioInputA6;	// SPI1_MISO [L3GD20_SA0/SDO]

using SpiMaster = SpiMaster1;
using Transport = xpcc::Lis3TransportSpi< SpiMaster, Cs >;
using Gyroscope = xpcc::L3gd20< Transport >;
}


namespace lsm3
{
using Drdy = GpioInputE2;	// DRDY [LSM303DLHC_DRDY]: GPXTI2
using Int1 = GpioInputE4;	// MEMS_INT3 [LSM303DLHC_INT1]: GPXTI4
using Int2 = GpioInputE5;	// MEMS_INT4 [LSM303DLHC_INT2]: GPXTI5

using Scl = GpioB6;	// I2C1_SCL [LSM303DLHC_SCL]: I2C1_SCL
using Sda = GpioB7;	// I2C1_SDA [LSM303DLHC_SDA]: I2C1_SDA

// Hardware I2C not yet implemented for F3!
//using I2cMaster = I2cMaster1;
using I2cMaster = xpcc::SoftwareI2cMaster<GpioB6, GpioB7>;
using Accelerometer = xpcc::Lsm303a< I2cMaster >;
}


namespace usb
{
using Dm = GpioA11;		// DM: USB_DM
using Dp = GpioA12;		// DP: USB_DP
//using Device = UsbFs;
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	LedNorth::setOutput(xpcc::Gpio::Low);
	LedNorthEast::setOutput(xpcc::Gpio::Low);
	LedEast::setOutput(xpcc::Gpio::Low);
	LedSouthEast::setOutput(xpcc::Gpio::Low);
	LedSouth::setOutput(xpcc::Gpio::Low);
	LedSouthWest::setOutput(xpcc::Gpio::Low);
	LedWest::setOutput(xpcc::Gpio::Low);
	LedNorthWest::setOutput(xpcc::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}


inline void
initializeL3g()
{
	l3g::Int1::setInput();
	l3g::Int1::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	l3g::Int1::enableExternalInterrupt();
//	l3g::Int1::enableExternalInterruptVector(12);

	l3g::Int2::setInput();
	l3g::Int2::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	l3g::Int2::enableExternalInterrupt();
//	l3g::Int2::enableExternalInterruptVector(12);

	l3g::Cs::setOutput(xpcc::Gpio::High);

	l3g::Sck::connect(l3g::SpiMaster::Sck);
	l3g::Mosi::connect(l3g::SpiMaster::Mosi);
	l3g::Miso::connect(l3g::SpiMaster::Miso);

	l3g::SpiMaster::initialize<systemClock, 9000000>();
	l3g::SpiMaster::setDataMode(l3g::SpiMaster::DataMode::Mode3);
}


inline void
initializeLsm3()
{
	lsm3::Int1::setInput();
	lsm3::Int1::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	lsm3::Int1::enableExternalInterrupt();
//	lsm3::Int1::enableExternalInterruptVector(12);

	lsm3::Int2::setInput();
	lsm3::Int2::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	lsm3::Int2::enableExternalInterrupt();
//	lsm3::Int2::enableExternalInterruptVector(12);

	lsm3::Drdy::setInput();
	lsm3::Drdy::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	lsm3::Drdy::enableExternalInterrupt();
//	lsm3::Drdy::enableExternalInterruptVector(12);

	lsm3::Scl::connect(lsm3::I2cMaster::Scl);
	lsm3::Sda::connect(lsm3::I2cMaster::Sda);
	lsm3::I2cMaster::initialize<systemClock, 400000>();
}


/// not supported yet, due to missing USB driver
inline void
initializeUsb()
{
//	usb::Dm::connect(usb::Device::Dm);
//	usb::Dp::connect(usb::Device::Dp);
}

}

#endif	// XPCC_STM32_F3_DISCOVERY_HPP
