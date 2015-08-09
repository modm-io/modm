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

using namespace xpcc::stm32;


namespace Board
{

/// STM32F3 running at 72MHz generated from the external 8MHz clock
/// supplied by the on-board st-link
typedef SystemClock<Pll<ExternalClock<MHz8>, MHz72> > systemClock;


typedef GpioInputA0   Button;
typedef GpioOutputA8  ClockOut;


typedef GpioOutputE9  LedNorth;			// User LED 3: Red
typedef GpioOutputE10 LedNorthEast;		// User LED 5: Orange
typedef GpioOutputE11 LedEast;			// User LED 7: Green
typedef GpioOutputE12 LedSouthEast;		// User LED 9: Blue
typedef GpioOutputE13 LedSouth;			// User LED 10: Red
typedef GpioOutputE14 LedSouthWest;		// User LED 8: Orange
typedef GpioOutputE15 LedWest;			// User LED 6: Green
typedef GpioOutputE8  LedNorthWest;		// User LED 4: Blue


namespace l3g
{
typedef GpioInputE0		Int1;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
typedef GpioInputE1		Int2;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

typedef GpioOutputE3	Cs;		// CS_I2C/SPI [L3GD20_CS_I2C/SPI]: GPIO_Output
typedef GpioInputA6		Miso;	// SPI1_MISO [L3GD20_SA0/SDO]: SPI1_MISO
typedef GpioOutputA5	Sck;	// SPI1_SCK [L3GD20_SCL/SPC]: SPI1_SCK
typedef GpioOutputA7	Mosi;	// SPI1_MISO [L3GD20_SDA/SDI/SDO]: SPI1_MOSI

typedef SpiMaster1 SpiMaster;
}


namespace lsm3
{
typedef GpioInputE2	Drdy;	// DRDY [LSM303DLHC_DRDY]: GPXTI2
typedef GpioInputE4	Int1;	// MEMS_INT3 [LSM303DLHC_INT1]: GPXTI4
typedef GpioInputE5	Int2;	// MEMS_INT4 [LSM303DLHC_INT2]: GPXTI5

typedef GpioB6		Scl;	// I2C1_SCL [LSM303DLHC_SCL]: I2C1_SCL
typedef GpioB7		Sda;	// I2C1_SDA [LSM303DLHC_SDA]: I2C1_SDA

// Hardware I2C not yet implemented for F3!
//typedef I2cMaster1 I2cMaster;
typedef xpcc::SoftwareI2cMaster<GpioB6, GpioB7> I2cMaster;
}


namespace usb
{
typedef GpioA11	Dm;		// DM: USB_DM
typedef GpioA12	Dp;		// DP: USB_DP
//typedef UsbFs Device;
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

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

	lsm3::Int2::setInput();
	lsm3::Int2::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	lsm3::Int2::enableExternalInterrupt();
//	lsm3::Int2::enableExternalInterruptVector(12);

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
