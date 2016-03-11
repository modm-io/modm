// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// 32F072DISCOVERY
// Discovery kit for STM32F072 series
// http://www.st.com/web/en/catalog/tools/FM116/SC959/SS1532/PF259724
//

#ifndef XPCC_STM32_F072_DISCOVERY_HPP
#define XPCC_STM32_F072_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/inertial/l3gd20.hpp>

using namespace xpcc::stm32;


namespace Board
{

/// STM32F072 running at 48MHz generated from the internal 48MHz clock
// TODO: enable once clock driver is implemented
//using systemClock = SystemClock<InternalClock<MHz48>, MHz48>;

struct systemClock
{
	static constexpr int Frequency = MHz48;
	static constexpr int Usart1 = Frequency;
	static constexpr int Can1 = Frequency;
	static constexpr int Spi2 = Frequency;

	static bool inline
	enable()
	{
		// Enable the interal 48MHz clock
		ClockControl::enableInternalClockMHz48();
		// set flash latency for 48MHz
		ClockControl::setFlashLatency(Frequency);
		// Switch to the 48MHz clock
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::InternalClockMHz48);
		// update frequencies for busy-wait delay functions
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(4000 / (Frequency / 1000000));

		return true;
	}
};


using Button   = GpioInputA0;

using LedUp    = GpioOutputC6;
using LedDown  = GpioOutputC7;
using LedLeft  = GpioOutputC8;
using LedRight = GpioOutputC9;


namespace l3g
{
using Int1 = GpioInputC1;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
using Int2 = GpioInputC2;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

using Cs   = GpioOutputC0;		// CS_I2C/SPI [L3GD20_CS_I2C/SPI]
using Sck  = GpioOutputB13;	// SPI2_SCK [L3GD20_SCL/SPC]
using Mosi = GpioOutputB15;	// SPI2_MISO [L3GD20_SDA/SDI/SDO]
using Miso = GpioInputB14;	// SPI2_MISO [L3GD20_SA0/SDO]

using SpiMaster = SpiMaster2;
using Transport = xpcc::Lis3TransportSpi< SpiMaster, Cs >;
using Gyroscope = xpcc::L3gd20< Transport >;
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	LedUp::setOutput(xpcc::Gpio::Low);
	LedDown::setOutput(xpcc::Gpio::Low);
	LedLeft::setOutput(xpcc::Gpio::Low);
	LedRight::setOutput(xpcc::Gpio::Low);

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

	l3g::SpiMaster::initialize<systemClock, 3000000>();
	l3g::SpiMaster::setDataMode(l3g::SpiMaster::DataMode::Mode3);
}

} // namespace Board

#endif	// XPCC_STM32_F072_DISCOVERY_HPP
