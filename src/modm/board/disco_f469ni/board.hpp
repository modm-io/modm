/*
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2018, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_F469_DISCOVERY_HPP
#define MODM_STM32_F469_DISCOVERY_HPP

#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/ui/display/color_graphic_display.hpp>
#include <modm/driver/touch/ft6x06.hpp>
#include <modm/debug/logger.hpp>

/// @ingroup modm_board_disco_f469ni
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_disco_f469ni
namespace Board
{
	using namespace modm::literals;

/// STM32F469 running at 180MHz from the external 8MHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 180_MHz;
	static constexpr uint32_t Apb1 = Frequency / 4;
	static constexpr uint32_t Apb2 = Frequency / 2;

	static constexpr uint32_t Adc = Apb2;

	static constexpr uint32_t Spi1 = Apb2;
	static constexpr uint32_t Spi2 = Apb1;
	static constexpr uint32_t Spi3 = Apb1;
	static constexpr uint32_t Spi4 = Apb2;
	static constexpr uint32_t Spi5 = Apb2;
	static constexpr uint32_t Spi6 = Apb2;

	static constexpr uint32_t Usart1 = Apb2;
	static constexpr uint32_t Usart2 = Apb1;
	static constexpr uint32_t Usart3 = Apb1;
	static constexpr uint32_t Uart4  = Apb1;
	static constexpr uint32_t Uart5  = Apb1;
	static constexpr uint32_t Usart6 = Apb2;
	static constexpr uint32_t Uart7  = Apb1;
	static constexpr uint32_t Uart8  = Apb1;

	static constexpr uint32_t Can1 = Apb1;
	static constexpr uint32_t Can2 = Apb1;

	static constexpr uint32_t I2c1 = Apb1;
	static constexpr uint32_t I2c2 = Apb1;
	static constexpr uint32_t I2c3 = Apb1;
	static constexpr uint32_t I2c4 = Apb1;

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
	static constexpr uint32_t Timer10 = Apb2Timer;
	static constexpr uint32_t Timer11 = Apb2Timer;
	static constexpr uint32_t Timer12 = Apb1Timer;
	static constexpr uint32_t Timer13 = Apb1Timer;
	static constexpr uint32_t Timer14 = Apb1Timer;

	static constexpr uint32_t Usb = 48_MHz;

	static bool inline
	enable()
	{
		Rcc::enableExternalCrystal(); // 8 MHz
		const Rcc::PllFactors pllFactors{
			.pllM = 8,		// 8MHz / M=8 -> 1MHz   !!! Must be 1 MHz for PLLSAI !!!
			.pllN = 360,	// 1MHz * N=360 -> 360MHz
			.pllP = 2,		// 360MHz / P=2 -> 180MHz = F_cpu
		};
		Rcc::enablePll(Rcc::PllSource::ExternalCrystal, pllFactors);
		// Required for 180 MHz clock
		Rcc::enableOverdriveMode();
		Rcc::setFlashLatency<Frequency>();
		Rcc::enableSystemClock(Rcc::SystemClockSource::Pll);
		Rcc::setApb1Prescaler(Rcc::Apb1Prescaler::Div4);
		Rcc::setApb2Prescaler(Rcc::Apb2Prescaler::Div2);
		Rcc::updateCoreFrequency<Frequency>();

		{
			// LCD clock configuration
			// PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz
			// PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 384 Mhz
			// PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 384 MHz / 7 = 54.857 MHz
			// LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 54.857 MHz / 2 = 27.429 MHz
			RCC->PLLSAICFGR = (7 << 28) | (15 << 24) | (3 << 16) | (384 << 6);
			// Select PLLSAI clock for 48MHz clocks
			RCC->DCKCFGR |= RCC_DCKCFGR_CK48MSEL;
			// Enable PLLSAI
			RCC->CR |= RCC_CR_PLLSAION;
			for (int t = 1'024; not (RCC->CR & RCC_CR_PLLSAIRDY) and t; t--) {
				modm::delay_ms(1);
			}
		}

		return true;
	}
};

// Arduino Footprint
using A0 = GpioB1;
using A1 = GpioC2;
using A2 = GpioC3;
using A3 = GpioC4;
using A4 = GpioC5;
using A5 = GpioA4;

using D0  = GpioG9;
using D1  = GpioG14;
using D2  = GpioG13;
using D3  = GpioA1;
using D4  = GpioG12;
using D5  = GpioA2;
using D6  = GpioA6;
using D7  = GpioG11;
using D8  = GpioG10;
using D9  = GpioA7;
using D10 = GpioH6;
using D11 = GpioB15;
using D12 = GpioB14;
using D13 = GpioD3;
using D14 = GpioB9;
using D15 = GpioB8;

using Button = GpioInputA0;

using LedGreen  = GpioInverted<GpioOutputG6>;		// LED1 [Green]
using LedOrange = GpioInverted<GpioOutputD4>;		// LED2 [Orange]
using LedRed    = GpioInverted<GpioOutputD5>;		// LED3 [Red]
using LedBlue   = GpioInverted<GpioOutputK3>;		// LED4 [Blue]
using LedUsb    = GpioInverted<GpioOutputB7>;		// LED5 [Red] USB Overcurrent
using LedD13    = GpioOutputD3;							// LED7 [Green]

using Leds = SoftwareGpioPort< LedBlue, LedRed, LedOrange, LedGreen >;

using DisplayReset = GpioOutputH7;

namespace ft6
{
using Int = GpioInputJ5;
using Scl = GpioB8;
using Sda = GpioB9;
using I2cMaster = I2cMaster1;
using Touch = modm::Ft6x06< I2cMaster >;
}

namespace usb
{
using Vbus = GpioA9;
using Id = GpioA10;
using Dm = GpioA11;
using Dp = GpioA12;

using Overcurrent = GpioInputB7;	// OTG_FS_OverCurrent
using Power = GpioOutputB2;			// OTG_FS_PowerSwitchOn

using Device = UsbFs;
}

namespace stlink
{
using Tx = GpioOutputB10;		// STLK_RX [STLINK V2-1_U2_RX]: USART3_TX
using Rx = GpioInputB11;		// STLK_TX [STLINK V2-1_U2_TX]: USART3_RX
using Uart = Usart3;
}

using LoggerDevice = modm::IODeviceWrapper< stlink::Uart, modm::IOBuffer::BlockIfFull >;

inline void
initializeTouchscreen()
{
	ft6::Int::setInput();
	ft6::Int::setInputTrigger(Gpio::InputTrigger::FallingEdge);
	ft6::Int::enableExternalInterrupt();
//	ft6::Int::enableExternalInterruptVector(12);

	ft6::I2cMaster::connect<ft6::Scl::Scl, ft6::Sda::Sda>();
	ft6::I2cMaster::initialize<SystemClock, 360_kHz>();
}

void
initializeDisplay();

void
setDisplayBuffer(void * buffer);

void *
getDisplayBuffer();

modm::ColorGraphicDisplay&
getDisplay();

inline void
initialize()
{
	// initialized in `modm_board_init()`
	// SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<SystemClock, 115200_Bd>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);
	LedOrange::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

inline void
initializeUsbFs()
{
	usb::Device::initialize<SystemClock>();
	usb::Device::connect<usb::Dm::Dm, usb::Dp::Dp, usb::Id::Id>();

	usb::Overcurrent::setInput();
	usb::Vbus::setInput();
	// Enable VBUS sense (B device) via pin PA9
	USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;
}

}

#endif	// MODM_STM32_F469_DISCOVERY_HPP
