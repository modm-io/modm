// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F429IDISCOVERY
// Discovery kit for STM32F429 line
// http://www.st.com/web/catalog/tools/FM116/SC959/SS1532/PF259090
//

#ifndef XPCC_STM32_F429_DISCOVERY_HPP
#define XPCC_STM32_F429_DISCOVERY_HPP

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::stm32;

namespace Board
{

/// STM32F429 running at 168MHz (USB Clock qt 48MHz) generated from the
/// external on-board 8MHz crystal
typedef SystemClock<Pll<ExternalCrystal<MHz8>, MHz168, MHz48> > systemClock;


typedef GpioInputA0 Button;


typedef GpioOutputG13 LedGreen;	// User LED (LD3)
typedef GpioOutputG14 LedRed;	// User LED (LD4)


namespace l3g
{
typedef GpioInputA1		Int1;	// MEMS_INT1 [L3GD20_INT1]: GPXTI0
typedef GpioInputA2		Int2;	// MEMS_INT2 [L3GD20_DRDY/INT2]: GPXTI1

typedef GpioOutputC1	Cs;		// CS_I2C/SPI [L3GD20_CS_I2C/SPI]
typedef GpioOutputF7	Sck;	// SPI5_SCK [L3GD20_SCL/SPC]
typedef GpioOutputF9	Mosi;	// SPI5_MISO [L3GD20_SDA/SDI/SDO]
typedef GpioInputF8		Miso;	// SPI5_MISO [L3GD20_SA0/SDO]

typedef SpiMaster5 SpiMaster;
}


namespace fmc
{
typedef GpioOutputB5	Sdcke1;	// SDCKE1: FMC_SDCKE1
typedef GpioOutputG8	Sdclk;	// SDCLK: FMC_SDCLK
typedef GpioOutputG15	Sdncas;	// SDNCAS: FMC_SDNCAS
typedef GpioOutputF11	Sdnras;	// SDNRAS: FMC_SDNRAS
typedef GpioOutputC0	Sdnwe;	// SDNWE: FMC_SDNWE

typedef GpioOutputB6	Cs;		// SDNE1 [SDRAM_CS]: FMC_SDNE1
typedef GpioOutputE0	Ldqm;	// NBL0 [SDRAM_LDQM]: FMC_NBL0
typedef GpioOutputE1	Udqm;	// NBL1 [SDRAM_UDQM]: FMC_NBL1

typedef GpioOutputG4	Ba0;	// BA0: FMC_A14_BA0
typedef GpioOutputG5	Ba1;	// BA1: FMC_A15_BA1

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
typedef GpioOutputG0	A10;	// A10: FMC_A10
typedef GpioOutputG1	A11;	// A11: FMC_A11

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
}


namespace lcd
{
typedef GpioInputA15	TouchInt;	// TP_INT1 [Touch Panel]: GPXTI15
typedef GpioA8			TouchScl;	// I2C3_SCL [ACP/RF_SCL]: I2C3_SCL
typedef GpioC9			TouchSda;	// I2C3_SDA [ACP/RF_SDA]: I2C3_SDA
typedef GpioOutputA7	TouchRst;	// ACP_RST: GPIO_Output

typedef GpioOutputC6	Hsync;	// HSYNC: LTDC_HSYNC
typedef GpioOutputA4	Vsync;	// VSYNC: LTDC_VSYNC

typedef GpioOutputC2	Csx;	// CSX
typedef GpioOutputD12	Rdx;	// RDX
typedef GpioOutputD13	WrxDcx;	// WRX_DCX
typedef GpioInputD11	Te;		// TE

typedef GpioOutputF10	Enable;	// ENABLE: LTDC_DE
typedef GpioOutputG7	DotClk;	// DOTCLK: LTDC_CLK

typedef GpioOutputC10	R2;		// R2: LTDC_R2
typedef GpioOutputB0	R3;		// R3: LTDC_R3
typedef GpioOutputA11	R4;		// R4: LTDC_R4
typedef GpioOutputA12	R5;		// R5: LTDC_R5
typedef GpioOutputB1	R6;		// R6: LTDC_R6
typedef GpioOutputG6	R7;		// R7: LTDC_R7

typedef GpioOutputA6	G2;		// G2: LTDC_G2
typedef GpioOutputG10	G3;		// G3: LTDC_G3
typedef GpioOutputB10	G4;		// G4: LTDC_G4
typedef GpioOutputB11	G5;		// G5: LTDC_G5
typedef GpioOutputC7	G6;		// G6: LTDC_G6
typedef GpioOutputD3	G7;		// G7: LTDC_G7

typedef GpioOutputD6	B2;		// B2: LTDC_B2
typedef GpioOutputG11	B3;		// B3: LTDC_B3
typedef GpioOutputG12	B4;		// B4: LTDC_B4
typedef GpioOutputA3	B5;		// B5: LTDC_B5
typedef GpioOutputB8	B6;		// B6: LTDC_B6
typedef GpioOutputB9	B7;		// B7: LTDC_B7
}


namespace usb
{
typedef GpioOutputB14	Dm;		// OTG_FS_DM: USB_OTG_HS_DM
typedef GpioOutputB15	Dp;		// OTG_FS_DP: USB_OTG_HS_DP
typedef GpioOutputB12	Id;		// OTG_FS_ID: USB_OTG_HS_ID

typedef GpioC5			Overcurrent;	// OTG_FS_OC [OTG_FS_OverCurrent]: GPXTI5
typedef GpioOutputC4	Power;			// OTG_FS_PSO [OTG_FS_PowerSwitchOn]
typedef GpioB13			VBus;			// VBUS_FS: USB_OTG_HS_VBUS
//typedef UsbFs Device;
}


inline void
initialize()
{
	systemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::Low);

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

	l3g::SpiMaster::initialize<systemClock, MHz10>();
	l3g::SpiMaster::setDataMode(l3g::SpiMaster::DataMode::Mode3);
}

}

#endif	// XPCC_STM32_F429_DISCOVERY_HPP
