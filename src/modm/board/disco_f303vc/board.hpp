/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// STM32F3DISCOVERY
// Discovery kit for STM32 F3 series
// http://www.st.com/en/evaluation-tools/stm32f3discovery.html
//

#ifndef MODM_STM32_F3_DISCOVERY_HPP
#define MODM_STM32_F3_DISCOVERY_HPP

#include <modm/platform/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/inertial/l3gd20.hpp>
#include <modm/driver/inertial/lsm303a.hpp>

using namespace modm::platform;


namespace Board
{

/* SystemClock generator is only available for selected STM32F4 devices.
 * The idea is that it is generated automatically for you like the rest of the
 * HAL, however, modm does not have this capability yet. See PR #36.
 */
// using systemClock = SystemClock<Pll<ExternalClock<MHz8>, MHz72> >;

// Instead this manual implementation of the system clock is used:
/// STM32F303 running at 72MHz generated from the external 8MHz clock
/// supplied by the on-board st-link
struct systemClock {
	static constexpr uint32_t Frequency = 72 * MHz1;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 2;
	static constexpr uint32_t Apb2 = Frequency;

	static constexpr uint32_t Adc1   = Apb2;
	static constexpr uint32_t Adc2   = Apb2;
	static constexpr uint32_t Adc3   = Apb2;
	static constexpr uint32_t Adc4   = Apb2;

	static constexpr uint32_t Can    = Apb1;

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
	static constexpr uint32_t Apb2Timer = Apb2 * 1;
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
	static constexpr uint32_t Timer15 = Apb2Timer;
	static constexpr uint32_t Timer16 = Apb2Timer;
	static constexpr uint32_t Timer17 = Apb2Timer;

	static bool inline
	enable()
	{
		ClockControl::enableExternalClock();	// 8MHz
		ClockControl::enablePll(
			ClockControl::PllSource::ExternalClock,
			ClockControl::UsbPrescaler::Div1_5,
			9,
			1
		);
		// set flash latency for 72MHz
		ClockControl::setFlashLatency(Frequency);
		// switch system clock to PLL output
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setAhbPrescaler(ClockControl::AhbPrescaler::Div1);
		// APB1 has max. 36MHz
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div2);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div1);
		// update frequencies for busy-wait delay functions
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

		return true;
	}
};


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

using Leds = SoftwareGpioPort< LedNorthWest, LedWest, LedSouthWest, LedSouth, LedSouthEast, LedEast, LedNorthEast, LedNorth >;


namespace l3g
{
using Int1 = GpioInputE0;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
using Int2 = GpioInputE1;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

using Cs   = GpioOutputE3;	// CS_I2C/SPI [L3GD20_CS_I2C/SPI]
using Sck  = GpioOutputA5;	// SPI1_SCK [L3GD20_SCL/SPC]
using Mosi = GpioOutputA7;	// SPI1_MISO [L3GD20_SDA/SDI/SDO]
using Miso = GpioInputA6;	// SPI1_MISO [L3GD20_SA0/SDO]

using SpiMaster = SpiMaster1;
using Transport = modm::Lis3TransportSpi< SpiMaster, Cs >;
using Gyroscope = modm::L3gd20< Transport >;
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
using I2cMaster = BitBangI2cMaster<Scl, Sda>;
using Accelerometer = modm::Lsm303a< I2cMaster >;
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
	modm::cortex::SysTickTimer::initialize<systemClock>();

	LedNorth::setOutput(modm::Gpio::Low);
	LedNorthEast::setOutput(modm::Gpio::Low);
	LedEast::setOutput(modm::Gpio::Low);
	LedSouthEast::setOutput(modm::Gpio::Low);
	LedSouth::setOutput(modm::Gpio::Low);
	LedSouthWest::setOutput(modm::Gpio::Low);
	LedWest::setOutput(modm::Gpio::Low);
	LedNorthWest::setOutput(modm::Gpio::Low);

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

	lsm3::I2cMaster::connect<lsm3::Scl::BitBang, lsm3::Sda::BitBang>();
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

#endif	// MODM_STM32_F3_DISCOVERY_HPP
