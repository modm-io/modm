/*
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

//
// STM32F469I-DISCO
// Discovery kit for STM32F469 line
// http://www.st.com/en/evaluation-tools/32f469idiscovery.html
//

#ifndef MODM_STM32_F469_DISCOVERY_HPP
#define MODM_STM32_F469_DISCOVERY_HPP

#include <modm/platform/platform.hpp>
#include <modm/ui/display/graphic_display.hpp>
#include <modm/driver/touch/ft6x06.hpp>
#include <modm/debug/logger.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

namespace Board
{

/// STM32F469 running at 180MHz from the external 8MHz crystal
struct systemClock
{
	static constexpr uint32_t Frequency = MHz180;
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

	static bool inline
	enable()
	{
		ClockControl::enableExternalCrystal(); // 8 MHz
		ClockControl::enablePll(
		ClockControl::PllSource::ExternalCrystal,
			8,      // 8MHz / N=8 -> 1MHz   !!! Must be 1 MHz for PLLSAI !!!
			360,    // 1MHz * M=360 -> 360MHz
			2,      // 360MHz / P=2 -> 180MHz = F_cpu
			7       // 360MHz / Q=7 -> ~51MHz (value ignored! PLLSAI generates 48MHz for F_usb)
		);
		// Enable overdrive mode
		PWR->CR |= PWR_CR_ODEN;
		// wait for it
		while (not (PWR->CSR & PWR_CSR_ODRDY)) ;
		ClockControl::setFlashLatency(Frequency);
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div4);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div2);
		// update clock frequencies
		modm::clock::fcpu     = Frequency;
		modm::clock::fcpu_kHz = Frequency / 1000;
		modm::clock::fcpu_MHz = Frequency / 1000000;
		modm::clock::ns_per_loop = ::round(3000.f / (Frequency / 1000000));

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

namespace stlink
{
using Tx = GpioOutputB10;		// STLK_RX [STLINK V2-1_U2_RX]: USART3_TX
using Rx = GpioInputB11;		// STLK_TX [STLINK V2-1_U2_TX]: USART3_RX
using Uart = Usart3;
}

inline void
initializeTouchscreen()
{
	ft6::Int::setInput();
	ft6::Int::setInputTrigger(Gpio::InputTrigger::FallingEdge);
	ft6::Int::enableExternalInterrupt();
//	ft6::Int::enableExternalInterruptVector(12);

	ft6::I2cMaster::connect<ft6::Scl::Scl, ft6::Sda::Sda>();
	ft6::I2cMaster::initialize<systemClock, 360000>();
}

void
initializeDisplay();

void
setDisplayBuffer(void * buffer);

void *
getDisplayBuffer();

modm::GraphicDisplay&
getDisplay();

inline void
initialize()
{
	// initialized in `modm_board_init()`
	// systemClock::enable();
	modm::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Uart::connect<stlink::Tx::Tx, stlink::Rx::Rx>();
	stlink::Uart::initialize<systemClock, 115200>();

	LedGreen::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);
	LedOrange::setOutput(modm::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// MODM_STM32_F469_DISCOVERY_HPP
