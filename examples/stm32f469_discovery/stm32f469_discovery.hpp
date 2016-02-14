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

using namespace xpcc::stm32;

namespace Board
{

/// STM32F469 running at 168MHz (USB Clock qt 48MHz) generated from the
/// external on-board 8MHz crystal
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> > systemClock;

typedef GpioInputA0 Button;

typedef xpcc::GpioInverted<GpioOutputG6> LedGreen;		// LED1 [Green]
typedef xpcc::GpioInverted<GpioOutputD4> LedOrange;		// LED2 [Orange]
typedef xpcc::GpioInverted<GpioOutputD5> LedRed;		// LED3 [Red]
typedef xpcc::GpioInverted<GpioOutputK3> LedBlue;		// LED4 [Blue]
typedef GpioOutputD3 LedD13;							// LED7 [Green]

namespace stlink
{
typedef GpioOutputB10	Tx;		// STLK_RX [STLINK V2-1_U2_RX]: USART3_TX
typedef GpioInputB11	Rx;		// STLK_TX [STLINK V2-1_U2_TX]: USART3_RX
typedef Usart3 Uart;
}

namespace fmc
{
typedef GpioOutputH2	Sdcke;		// SDCKE0 [MT48LC4M32B2B5-6A_CKE]: FMC_SDCKE0
typedef GpioOutputG8	Sdclk;		// SDCLK [MT48LC4M32B2B5-6A_CLK]: FMC_SDCLK
typedef GpioOutputG15	Sdncas;		// SDNCAS [MT48LC4M32B2B5-6A_CAS]: FMC_SDNCAS
typedef GpioOutputF11	Sdnras;		// SDNRAS [MT48LC4M32B2B5-6A_RAS]: FMC_SDNRAS
typedef GpioOutputC0	Sdnwe;		// SDNWE [MT48LC4M32B2B5-6A_WE]: FMC_SDNWE
typedef GpioOutputH3	Sdne;		// SDNE0 [MT48LC4M32B2B5-6A_CS]: FMC_SDNE0

typedef GpioOutputE0	Dqm0;		// FMC_NBL0 [MT48LC4M32B2B5-6A_DQM0]: FMC_NBL0
typedef GpioOutputE1	Dqm1;		// FMC_NBL1 [MT48LC4M32B2B5-6A_DQM1]: FMC_NBL1
typedef GpioOutputI4	Dqm2;		// FMC_NBL2 [MT48LC4M32B2B5-6A_DQM2]: FMC_NBL2
typedef GpioOutputI5	Dqm3;		// FMC_NBL3 [MT48LC4M32B2B5-6A_DQM3]: FMC_NBL3

typedef GpioOutputG4	Ba0;		// FMC_A14_BA0: FMC_A14_BA0
typedef GpioOutputG5	Ba1;		// FMC_A15_BA1: FMC_A15_BA1

typedef GpioOutputF0	A0;		// A0: FMC_A0
typedef GpioOutputF1	A1;		// A1: FMC_A1
typedef GpioOutputF2	A2;		// A2: FMC_A2
typedef GpioOutputF3	A3;		// A3: FMC_A3
typedef GpioOutputF4	A4;		// A4: FMC_A4
typedef GpioOutputF5	A5;		// A5: FMC_A5
typedef GpioOutputF12	A6;		// A6: FMC_A6
typedef GpioOutputF13	A7;		// A7: FMC_A7
typedef GpioOutputF14	A8;		// A8: FMC_A8
typedef GpioOutputF15	A9;		// A9: FMC_A9
typedef GpioOutputG0	A10;		// A10: FMC_A10
typedef GpioOutputG1	A11;		// A11: FMC_A11

typedef GpioOutputD14	D0;		// D0: FMC_D0_DA0
typedef GpioOutputD15	D1;		// D1: FMC_D1_DA1
typedef GpioOutputD0	D2;		// D2: FMC_D2_DA2
typedef GpioOutputD1	D3;		// D3: FMC_D3_DA3
typedef GpioOutputE7	D4;		// D4: FMC_D4_DA4
typedef GpioOutputE8	D5;		// D5: FMC_D5_DA5
typedef GpioOutputE9	D6;		// D6: FMC_D6_DA6
typedef GpioOutputE10	D7;		// D7: FMC_D7_DA7
typedef GpioOutputE11	D8;		// D8: FMC_D8_DA8
typedef GpioOutputE12	D9;		// D9: FMC_D9_DA9
typedef GpioOutputE13	D10;	// D10: FMC_D10_DA10
typedef GpioOutputE14	D11;	// D11: FMC_D11_DA11
typedef GpioOutputE15	D12;	// D12: FMC_D12_DA12
typedef GpioOutputD8	D13;	// D13: FMC_D13_DA13
typedef GpioOutputD9	D14;	// D14: FMC_D14_DA14
typedef GpioOutputD10	D15;	// D15: FMC_D15_DA15
typedef GpioOutputH8	D16;	// D16: FMC_D16
typedef GpioOutputH9	D17;	// D17: FMC_D17
typedef GpioOutputH10	D18;	// D18: FMC_D18
typedef GpioOutputH11	D19;	// D19: FMC_D19
typedef GpioOutputH12	D20;	// D20: FMC_D20
typedef GpioOutputH13	D21;	// D21: FMC_D21
typedef GpioOutputH14	D22;	// D22: FMC_D22
typedef GpioOutputH15	D23;	// D23: FMC_D23
typedef GpioOutputI0	D24;	// D24: FMC_D24
typedef GpioOutputI1	D25;	// D25: FMC_D25
typedef GpioOutputI2	D26;	// D26: FMC_D26
typedef GpioOutputI3	D27;	// D27: FMC_D27
typedef GpioOutputI6	D28;	// D28: FMC_D28
typedef GpioOutputI7	D29;	// D29: FMC_D29
typedef GpioOutputI9	D30;	// D30: FMC_D30
typedef GpioOutputI10	D31;	// D31: FMC_D31
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
