/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Antal Szabó
 * Copyright (c) 2020, Pavel Pletenev
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/platform.hpp>
#include <modm/debug.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/driver/storage/block_device_spiflash.hpp>

using namespace modm::platform;

/// @ingroup modm_board_stm32_f4ve
namespace Board
{
	using namespace modm::literals;

/// STM32F407 running at 168MHz generated from the external 8MHz crystal
struct SystemClock {
	static constexpr uint32_t HSExternalOscillatorFrequency = 8_MHz;
	static constexpr uint32_t LSExternalOscillatorFrequency = 32768_Hz;
	static constexpr uint32_t Frequency = 168_MHz;
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
		Rcc::enableExternalCrystal();	// 8MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 4,		// 8MHz / M=4 -> 2MHz
			.pllN = 168,	// 2MHz * N=168 -> 336MHz
			.pllP = 2		// 336MHz / P=2 -> 168MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);
		// set flash latency for 168MHz
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

using LoggerDevice = modm::IODeviceWrapper< Usart1 , modm::IOBuffer::BlockIfFull >;

using Button = GpioInputA0;
using ButtonK0 = GpioInputE4;
using ButtonK1 = GpioInputE5;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using LedGreen2 = GpioOutputA6;	// User LED 1
using LedGreen3  = GpioOutputA7;// User LED 2

using Leds = SoftwareGpioPort< LedGreen2, LedGreen3 >;

namespace sdcard
{

namespace sdio {
using Cmd = GpioOutputD2;
using Clk = GpioOutputC12;
using D0 = GpioOutputC8;
using D1 = GpioOutputC9;
using D2 = GpioOutputC10;
using D3 = GpioOutputC1; // CD
} // namespace sdio

namespace spi {
using Cs   = sdio::D3;
using Sck  = sdio::Clk;
using Mosi = sdio::D0;
using Miso = sdio::Cmd;
} // namespace spi

} // namespace sdcard


namespace display {
// this board has a 16-pin bus interface
// to an ILI9341 display
namespace fsmc {
using D15 = GpioD10;
using D14 = GpioD9;
using D13 = GpioD8;
using D12 = GpioE15;
using D11 = GpioE14;
using D10 = GpioE13;
using D9 = GpioE12;
using D8 = GpioE11;
using D7 = GpioE10;
using D6 = GpioE9;
using D5 = GpioE8;
using D4 = GpioE7;
using D3 = GpioD1;
using D2 = GpioD0;
using D1 = GpioD15;
using D0 = GpioD14;
using Noe = GpioD4;
using Nwe = GpioD5;
using A18 = GpioD13; // this is register select
using Ne1 = GpioD7;
} // namespace fsmc

namespace touch {
using Clk = GpioB13; // Touch
using Cs = GpioB12; // Touch
using Mosi = GpioB15; // Touch
using Miso = GpioB14; // Touch
using Pen = GpioC5; // Touch

using SpiMaster = SpiMaster2;
} // namespace touch

using Backlight = GpioB1; // LCD
// LCD reset is connected to MCU's reset
// and to physical reset line
using Reset = GpioUnused;

} // namespace display

namespace nrf24l01 {

using Ce = GpioB6;
using nCs = GpioOutputB7;
using Sck = GpioOutputB3;
using Mosi = GpioOutputB5;
using Miso = GpioB4;
using Irq = GpioB8;

} // namespace nrf24l01


namespace w25q16 {
// a W25Q16JV 2 MiB flash chip
using Cs = GpioOutputB0;
using Mosi = GpioInputB5;
using Miso = GpioOutputB4;
using Sck = GpioOutputB3;
// 3 	- 	nWP 	+3.3V rail
// 4 	- 	GND 	Ground plane
// 7 	- 	nHOLD 	+3.3V rail
// 8 	- 	VCC 	+3.3V rail
using SpiMaster = SpiMaster1;

constexpr uint32_t BlockSize = 256;
constexpr uint32_t MemorySize = 2*1024*1024; // 16 MiBits
using StorageDevice = modm::BdSpiFlash<SpiMaster, Cs, MemorySize>;

} // namespace w25q16

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
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen2::setOutput(modm::Gpio::Low);
	LedGreen3::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();

	ButtonK0::setInput();
	ButtonK0::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	// ButtonK0::enableExternalInterrupt();

	ButtonK1::setInput();
	ButtonK1::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	// ButtonK1::enableExternalInterrupt();
	Usart1::connect<GpioA9::Tx, GpioA10::Rx>();
	Usart1::initialize<Board::SystemClock, 115200_Bd>();
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

inline void
initializeW25q16()
{
	w25q16::Cs::setOutput(modm::Gpio::High);

	w25q16::SpiMaster::connect<w25q16::Sck::Sck, w25q16::Mosi::Mosi, w25q16::Miso::Miso>();
	w25q16::SpiMaster::initialize<SystemClock, 42_MHz>();
}


}

