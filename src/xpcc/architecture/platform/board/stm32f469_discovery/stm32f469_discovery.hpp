// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F469I-DISCO
// Discovery kit for STM32F469 line
// http://www.st.com/web/catalog/tools/FM116/CL1620/SC959/SS1532/LN1848/PF262395
//

#ifndef XPCC_STM32_F469_DISCOVERY_HPP
#define XPCC_STM32_F469_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#define XPCC_BOARD_HAS_LOGGER

using namespace xpcc::stm32;

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
			4,      // 8MHz / N=4 -> 2MHz
			180,    // 2MHz * M=180 -> 360MHz
			2,      // 360MHz / P=2 -> 180MHz = F_cpu
			7       // 360MHz / Q=7 -> ~51MHz = F_usb => bad for USB
		);
		ClockControl::setFlashLatency(Frequency);
		ClockControl::enableSystemClock(ClockControl::SystemClockSource::Pll);
		ClockControl::setApb1Prescaler(ClockControl::Apb1Prescaler::Div4);
		ClockControl::setApb2Prescaler(ClockControl::Apb2Prescaler::Div2);
		// update clock frequencies
		xpcc::clock::fcpu     = Frequency;
		xpcc::clock::fcpu_kHz = Frequency / 1000;
		xpcc::clock::fcpu_MHz = Frequency / 1000000;
		xpcc::clock::ns_per_loop = ::round(3000 / (Frequency / 1000000));

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

using LedGreen  = xpcc::GpioInverted<GpioOutputG6>;		// LED1 [Green]
using LedOrange = xpcc::GpioInverted<GpioOutputD4>;		// LED2 [Orange]
using LedRed    = xpcc::GpioInverted<GpioOutputD5>;		// LED3 [Red]
using LedBlue   = xpcc::GpioInverted<GpioOutputK3>;		// LED4 [Blue]
using LedUsb    = xpcc::GpioInverted<GpioOutputB7>;		// LED5 [Red] USB Overcurrent
using LedD13    = GpioOutputD3;							// LED7 [Green]

using Leds = xpcc::SoftwareGpioPort< LedBlue, LedRed, LedOrange, LedGreen >;


namespace stlink
{
using Tx = GpioOutputB10;		// STLK_RX [STLINK V2-1_U2_RX]: USART3_TX
using Rx = GpioInputB11;		// STLK_TX [STLINK V2-1_U2_TX]: USART3_RX
using Uart = Usart3;
}

namespace fmc
{
using Sdcke  = GpioOutputH2;	// SDCKE0 [MT48LC4M32B2B5-6A_CKE]: FMC_SDCKE0
using Sdclk  = GpioOutputG8;	// SDCLK [MT48LC4M32B2B5-6A_CLK]: FMC_SDCLK
using Sdncas = GpioOutputG15;	// SDNCAS [MT48LC4M32B2B5-6A_CAS]: FMC_SDNCAS
using Sdnras = GpioOutputF11;	// SDNRAS [MT48LC4M32B2B5-6A_RAS]: FMC_SDNRAS
using Sdnwe  = GpioOutputC0;	// SDNWE [MT48LC4M32B2B5-6A_WE]: FMC_SDNWE
using Sdne   = GpioOutputH3;	// SDNE0 [MT48LC4M32B2B5-6A_CS]: FMC_SDNE0

using Dqm0 = GpioOutputE0;		// FMC_NBL0 [MT48LC4M32B2B5-6A_DQM0]: FMC_NBL0
using Dqm1 = GpioOutputE1;		// FMC_NBL1 [MT48LC4M32B2B5-6A_DQM1]: FMC_NBL1
using Dqm2 = GpioOutputI4;		// FMC_NBL2 [MT48LC4M32B2B5-6A_DQM2]: FMC_NBL2
using Dqm3 = GpioOutputI5;		// FMC_NBL3 [MT48LC4M32B2B5-6A_DQM3]: FMC_NBL3

using Ba0 = GpioOutputG4;		// FMC_A14_BA0: FMC_A14_BA0
using Ba1 = GpioOutputG5;		// FMC_A15_BA1: FMC_A15_BA1

using A0  = GpioOutputF0;		// A0: FMC_A0
using A1  = GpioOutputF1;		// A1: FMC_A1
using A2  = GpioOutputF2;		// A2: FMC_A2
using A3  = GpioOutputF3;		// A3: FMC_A3
using A4  = GpioOutputF4;		// A4: FMC_A4
using A5  = GpioOutputF5;		// A5: FMC_A5
using A6  = GpioOutputF12;		// A6: FMC_A6
using A7  = GpioOutputF13;		// A7: FMC_A7
using A8  = GpioOutputF14;		// A8: FMC_A8
using A9  = GpioOutputF15;		// A9: FMC_A9
using A10 = GpioOutputG0;		// A10: FMC_A10
using A11 = GpioOutputG1;		// A11: FMC_A11

using D0  = GpioOutputD14;		// D0: FMC_D0_DA0
using D1  = GpioOutputD15;		// D1: FMC_D1_DA1
using D2  = GpioOutputD0;		// D2: FMC_D2_DA2
using D3  = GpioOutputD1;		// D3: FMC_D3_DA3
using D4  = GpioOutputE7;		// D4: FMC_D4_DA4
using D5  = GpioOutputE8;		// D5: FMC_D5_DA5
using D6  = GpioOutputE9;		// D6: FMC_D6_DA6
using D7  = GpioOutputE10;		// D7: FMC_D7_DA7
using D8  = GpioOutputE11;		// D8: FMC_D8_DA8
using D9  = GpioOutputE12;		// D9: FMC_D9_DA9
using D10 = GpioOutputE13;		// D10: FMC_D10_DA10
using D11 = GpioOutputE14;		// D11: FMC_D11_DA11
using D12 = GpioOutputE15;		// D12: FMC_D12_DA12
using D13 = GpioOutputD8;		// D13: FMC_D13_DA13
using D14 = GpioOutputD9;		// D14: FMC_D14_DA14
using D15 = GpioOutputD10;		// D15: FMC_D15_DA15
using D16 = GpioOutputH8;		// D16: FMC_D16
using D17 = GpioOutputH9;		// D17: FMC_D17
using D18 = GpioOutputH10;		// D18: FMC_D18
using D19 = GpioOutputH11;		// D19: FMC_D19
using D20 = GpioOutputH12;		// D20: FMC_D20
using D21 = GpioOutputH13;		// D21: FMC_D21
using D22 = GpioOutputH14;		// D22: FMC_D22
using D23 = GpioOutputH15;		// D23: FMC_D23
using D24 = GpioOutputI0;		// D24: FMC_D24
using D25 = GpioOutputI1;		// D25: FMC_D25
using D26 = GpioOutputI2;		// D26: FMC_D26
using D27 = GpioOutputI3;		// D27: FMC_D27
using D28 = GpioOutputI6;		// D28: FMC_D28
using D29 = GpioOutputI7;		// D29: FMC_D29
using D30 = GpioOutputI9;		// D30: FMC_D30
using D31 = GpioOutputI10;		// D31: FMC_D31
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::initialize<systemClock>();

	stlink::Tx::connect(stlink::Uart::Tx);
	stlink::Rx::connect(stlink::Uart::Rx, Gpio::InputType::PullUp);
	stlink::Uart::initialize<systemClock, 115200>(12);

	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::Low);
	LedBlue::setOutput(xpcc::Gpio::Low);
	LedOrange::setOutput(xpcc::Gpio::Low);

	Button::setInput();
	Button::setInputTrigger(Gpio::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
//	Button::enableExternalInterruptVector(12);
}

}

#endif	// XPCC_STM32_F469_DISCOVERY_HPP
