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

namespace Board
{
/// @ingroup modm_board_stm32_f4ve
/// @{
using namespace modm::literals;

/// STM32F407 running at 168MHz generated from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Hse = 8_MHz;
	static constexpr uint32_t Lse = 32.768_kHz;
	static constexpr Rcc::PllConfig pll
	{
		.M = 4,   //   8 MHz /   4 =   2 MHz
		.N = 168, //   2 MHz * 168 = 336 MHz
		.P = 2,   // 336 MHz /   2 = 168 MHz = F_cpu
		.Q = 7,   // 336 MHz /   7 =  48 MHz = F_usb
	};
	static constexpr uint32_t PllP = Hse / pll.M * pll.N / pll.P;
	static constexpr uint32_t PllQ = Hse / pll.M * pll.N / pll.Q;
	static_assert(PllP == Rcc::MaxFrequency);

	static constexpr uint32_t Frequency = PllP;
	static constexpr uint32_t Ahb = Frequency;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;
	static constexpr uint32_t Ahb1 = Ahb;
	static constexpr uint32_t Ahb2 = Ahb;

	static constexpr uint32_t Crc = Ahb1;
	static constexpr uint32_t Dma1 = Ahb1;
	static constexpr uint32_t Dma2 = Ahb1;
	static constexpr uint32_t Eth = Ahb1;
	static constexpr uint32_t Flash = Ahb1;

	static constexpr uint32_t Dcmi = Ahb2;
	static constexpr uint32_t Rng = Ahb2;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;
	static constexpr uint32_t Dac = Apb1;
	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2s2Ext = Apb1;
	static constexpr uint32_t I2s3Ext = Apb1;
	static constexpr uint32_t IwdgBus = Apb1;
	static constexpr uint32_t Pwr = Apb1;
	static constexpr uint32_t RtcBus = Apb1;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4 = Apb1;
	static constexpr uint32_t Uart5 = Apb1;
	static constexpr uint32_t Wwdg = Apb1;

	static constexpr uint32_t Adc = Apb2;
	static constexpr uint32_t Adc1 = Apb2;
	static constexpr uint32_t Adc2 = Apb2;
	static constexpr uint32_t Adc3 = Apb2;
	static constexpr uint32_t Adc123Common = Apb2;
	static constexpr uint32_t Exti = Apb2;
	static constexpr uint32_t Sdio = Apb2;
	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Syscfg = Apb2;
	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart6 = Apb2;

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

	static constexpr uint32_t Usb = PllQ;
	static constexpr uint32_t Iwdg = Rcc::LsiFrequency;
	static constexpr uint32_t Rtc = Lse;

	static bool inline
	enable()
	{
		Rcc::enableLseCrystal();
		Rcc::enableHseCrystal();

		Rcc::setFlashLatency<Frequency>();
		Rcc::updateCoreFrequency<Frequency>();

		Rcc::setAhbPrescaler(Rcc::AhbPrescaler::Div1);
		Rcc::setApb1Prescaler(Rcc::ApbPrescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::ApbPrescaler::Div2);

		Rcc::enablePll(Rcc::PllSource::Hse, pll);
		Rcc::enableSystemClock(Rcc::SystemClockSource::PllP);
		Rcc::setRealTimeClockSource(Rcc::RealTimeClockSource::Lse);

		return true;
	}
};

using Usart1 = BufferedUart<UsartHal1>;
using LoggerDevice = modm::IODeviceWrapper< Usart1 , modm::IOBuffer::BlockIfFull >;

using Button = GpioInputA0;
using ButtonK0 = GpioInputE4;
using ButtonK1 = GpioInputE5;
using ClockOut = GpioOutputA8;
using SystemClockOut = GpioOutputC9;

using LedGreen2 = GpioOutputA6;	// User LED 1
using LedGreen3  = GpioOutputA7;// User LED 2

using Leds = SoftwareGpioPort< LedGreen2, LedGreen3 >;
/// @}

namespace sdcard
{

namespace sdio
{
/// @ingroup modm_board_stm32_f4ve
/// @{
using Cmd = GpioOutputD2;
using Clk = GpioOutputC12;
using D0 = GpioOutputC8;
using D1 = GpioOutputC9;
using D2 = GpioOutputC10;
using D3 = GpioOutputC1; // CD
/// @}
} // namespace sdio

namespace spi
{
/// @ingroup modm_board_stm32_f4ve
/// @{
using Cs   = sdio::D3;
using Sck  = sdio::Clk;
using Mosi = sdio::D0;
using Miso = sdio::Cmd;
/// @}
} // namespace spi

} // namespace sdcard


namespace display
{
/// this board has a 16-pin bus interface
/// to an ILI9341 display
namespace fsmc
{
/// @ingroup modm_board_stm32_f4ve
/// @{
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
/// @}
} // namespace fsmc

namespace touch
{
/// @ingroup modm_board_stm32_f4ve
/// @{
using Clk = GpioB13; // Touch
using Cs = GpioB12; // Touch
using Mosi = GpioB15; // Touch
using Miso = GpioB14; // Touch
using Pen = GpioC5; // Touch

using SpiMaster = SpiMaster2;
/// @}
} // namespace touch

/// @ingroup modm_board_stm32_f4ve
/// @{
using Backlight = GpioB1; // LCD
/// LCD reset is connected to MCU's reset
/// and to physical reset line
using Reset = GpioUnused;
/// @}

} // namespace display

namespace nrf24l01
{
/// @ingroup modm_board_stm32_f4ve
/// @{
using Ce = GpioB6;
using nCs = GpioOutputB7;
using Sck = GpioOutputB3;
using Mosi = GpioOutputB5;
using Miso = GpioB4;
using Irq = GpioB8;
/// @}
} // namespace nrf24l01


namespace w25q16
{
/// @ingroup modm_board_stm32_f4ve
/// @{
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
/// @}
} // namespace w25q16

namespace usb
{
/// @ingroup modm_board_stm32_f4ve
/// @{
using Dm = GpioA11;			// OTG_FS_DM: USB_OTG_FS_DM
using Dp = GpioA12;			// OTG_FS_DP: USB_OTG_FS_DP
using Id = GpioA10;			// OTG_FS_ID: USB_OTG_FS_ID

using Overcurrent = GpioD5;	// OTG_FS_OverCurrent
using Power = GpioOutputC0;	// OTG_FS_PowerSwitchOn
using VBus = GpioInputA9;	// VBUS_FS: USB_OTG_HS_VBUS
//using Device = UsbFs;
/// @}
}

/// @ingroup modm_board_stm32_f4ve
/// @{
inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	LedGreen2::setOutput(modm::Gpio::Low);
	LedGreen3::setOutput(modm::Gpio::Low);

	Button::setInput();
	ButtonK0::setInput();
	ButtonK1::setInput();

	Usart1::connect<GpioA9::Tx, GpioA10::Rx>();
	Usart1::initialize<Board::SystemClock, 115200_Bd>();
}

inline void
initializeW25q16()
{
	w25q16::Cs::setOutput(modm::Gpio::High);

	w25q16::SpiMaster::connect<w25q16::Sck::Sck, w25q16::Mosi::Mosi, w25q16::Miso::Miso>();
	w25q16::SpiMaster::initialize<SystemClock, 42_MHz>();
}
/// @}


}

