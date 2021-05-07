/*
 * Copyright (c) 2016, Fabian Greif
 * Copyright (c) 2016, Kevin Läufer
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F072_DISCOVERY_HPP
#define MODM_STM32_F072_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/inertial/l3gd20.hpp>

using namespace modm::platform;

/// @ingroup modm_board_disco_f072rb
namespace Board
{
	using namespace modm::literals;

/// STM32F072 running at 48MHz generated from the internal 48MHz clock
// TODO: enable once clock driver is implemented
//using SystemClock = SystemClock<InternalClock<48_MHz>, 48_MHz>;

struct SystemClock
{
	static constexpr int Frequency = 48_MHz;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb = Frequency;

	static constexpr uint32_t Adc   = Apb;
	static constexpr uint32_t Can   = Apb;

	static constexpr uint32_t Spi1   = Apb;
	static constexpr uint32_t Spi2   = Apb;

	static constexpr uint32_t Usart1 = Apb;
	static constexpr uint32_t Usart2 = Apb;
	static constexpr uint32_t Usart3 = Apb;
	static constexpr uint32_t Usart4 = Apb;

	static constexpr uint32_t I2c1   = Apb;
	static constexpr uint32_t I2c2   = Apb;

	static constexpr uint32_t Timer1  = Apb;
	static constexpr uint32_t Timer2  = Apb;
	static constexpr uint32_t Timer3  = Apb;
	static constexpr uint32_t Timer6  = Apb;
	static constexpr uint32_t Timer7  = Apb;
	static constexpr uint32_t Timer14 = Apb;
	static constexpr uint32_t Timer15 = Apb;
	static constexpr uint32_t Timer16 = Apb;
	static constexpr uint32_t Timer17 = Apb;

	static constexpr uint32_t Usb = 48_MHz;

	static bool inline
	enable()
	{
		// Enable the internal 48MHz clock
		Rcc::enableInternalClockMHz48();
		// set flash latency for 48MHz
		Rcc::setFlashLatency<Frequency>();
		// Switch to the 48MHz clock
		Rcc::enableSystemClock(Rcc::SystemClockSource::InternalClockMHz48);
		// update frequencies for busy-wait delay functions
		Rcc::updateCoreFrequency<Frequency>();

		return true;
	}
};


using Button   = GpioInputA0;

using LedUp    = GpioOutputC6;
using LedDown  = GpioOutputC7;
using LedLeft  = GpioOutputC8;
using LedRight = GpioOutputC9;

using Leds = SoftwareGpioPort< LedLeft, LedDown, LedRight, LedUp >;


namespace l3g
{
using Int1 = GpioInputC1;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
using Int2 = GpioInputC2;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

using Cs   = GpioOutputC0;	// CS_I2C/SPI [L3GD20_CS_I2C/SPI]
using Sck  = GpioOutputB13;	// SPI2_SCK [L3GD20_SCL/SPC]
using Mosi = GpioOutputB15;	// SPI2_MOSI [L3GD20_SDA/SDI/SDO]
using Miso = GpioInputB14;	// SPI2_MISO [L3GD20_SA0/SDO]

using SpiMaster = SpiMaster2;
using Transport = modm::Lis3TransportSpi< SpiMaster, Cs >;
using Gyroscope = modm::L3gd20< Transport >;
}

namespace usb
{
using Dm = GpioA11;			// OTG_FS_DM: USB_OTG_FS_DM
using Dp = GpioA12;			// OTG_FS_DP: USB_OTG_FS_DP

using Device = UsbFs;
}


inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedUp::setOutput(modm::Gpio::Low);
	LedDown::setOutput(modm::Gpio::Low);
	LedLeft::setOutput(modm::Gpio::Low);
	LedRight::setOutput(modm::Gpio::Low);

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

	l3g::Cs::setOutput(modm::Gpio::High);

	l3g::SpiMaster::connect<l3g::Sck::Sck, l3g::Mosi::Mosi, l3g::Miso::Miso>();
	l3g::SpiMaster::initialize<SystemClock, 3_MHz>();
	l3g::SpiMaster::setDataMode(l3g::SpiMaster::DataMode::Mode3);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp>();
}

} // namespace Board

#endif	// MODM_STM32_F072_DISCOVERY_HPP
