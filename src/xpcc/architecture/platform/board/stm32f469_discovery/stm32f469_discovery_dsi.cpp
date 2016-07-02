// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
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

#include "stm32f469_discovery.hpp"

extern void
otm8009a_init(uint8_t);

// ---------------------------------- DISPLAY ----------------------------------
void
board_initialize_display(uint8_t ColorCoding)
{
	if (RCC->APB2ENR & RCC_APB2ENR_LTDCEN) return;
	// Enable clock to LTDC interface
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;
	RCC->APB2RSTR |=  RCC_APB2RSTR_LTDCRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_LTDCRST;
	// Enable clock to DSI interface
	RCC->APB2ENR |= RCC_APB2ENR_DSIEN;
	RCC->APB2RSTR |=  RCC_APB2RSTR_DSIRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_DSIRST;

	{
		// Expanded `HAL_DSI_Init()`:
		// Enable regulator
		DSI->WRPCR = DSI_WRPCR_REGEN;
		// Wait until stable
		for (int t = 1024; not (DSI->WISR & DSI_WISR_RRS) and t; t--) {
			xpcc::delayMilliseconds(1);
		}
		// Set up PLL and enable it
		DSI->WRPCR |= (0 << 16) | (2 << 11) | (125 << 2) | DSI_WRPCR_PLLEN;
		// Wait until stable
		for (int t = 1024; not (DSI->WISR & DSI_WISR_PLLLS) and t; t--) {
			xpcc::delayMilliseconds(1);
		}
		// D-PHY clock and digital enable
		DSI->PCTLR = DSI_PCTLR_CKE | DSI_PCTLR_DEN;
		// Clock lane configuration
		DSI->CLCR = DSI_CLCR_DPCC;
		// Configure the number of active data lanes
		DSI->PCONFR = 1;
		// Set the TX escape clock division factor
		DSI->CCR = 4;
		// Calculate the bit period in high-speed mode in unit of 0.25 ns (UIX4)
		// The equation is : UIX4 = IntegerPart( (1000/F_PHY_Mhz) * 4 )
		// Where : F_PHY_Mhz = (NDIV * HSE_Mhz) / (IDF * ODF)
		// Set the bit period in high-speed mode
		DSI->WPCR[0] = 8;
		// Disable all error interrupts and reset the Error Mask
		DSI->IER[0] = 0;
		DSI->IER[1] = 0;
	}

	constexpr uint32_t VSA = 12;
	constexpr uint32_t VBP = 12;
	constexpr uint32_t VFP = 12;
	constexpr uint32_t HSA = 120;
	constexpr uint32_t HBP = 120;
	constexpr uint32_t HFP = 120;
	constexpr uint32_t HACT = 800;
	constexpr uint32_t VACT = 480;
	const uint8_t pixel_size = (ColorCoding == 0) ? sizeof(uint32_t) : sizeof(uint16_t);
	constexpr float ClockRatio = 62500.f / 27429;

	{
		// Expanded `HAL_DSI_ConfigVideoMode()`
		// Select video mode by resetting CMDM and DSIM bits
		DSI->MCR = 0;
		DSI->WCFGR = 0;
		// Configure the video mode transmission type
		DSI->VMCR = 2;
		// Configure the video packet size
		DSI->VPCR = HACT;
		// Set the chunks number to be transmitted through the DSI link
		DSI->VCCR = 0;
		// Set the size of the null packet
		DSI->VNPCR = 0xFFF;
		// Select the virtual channel for the LTDC interface traffic
		DSI->LVCIDR = 0;
		// Configure the polarity of control signals
		DSI->LPCR = 0;
		// Select the color coding for the host
		DSI->LCOLCR = ColorCoding;
		// Select the color coding for the wrapper
		DSI->WCFGR = ColorCoding << 1;
		// Set the Horizontal Synchronization Active (HSA) in lane byte clock cycles
		DSI->VHSACR = HSA * ClockRatio;
		// Set the Horizontal Back Porch (HBP) in lane byte clock cycles
		DSI->VHBPCR = HBP * ClockRatio;
		// Set the total line time (HLINE=HSA+HBP+HACT+HFP) in lane byte clock cycles
		DSI->VLCR = (HACT + HSA + HBP + HFP) * ClockRatio;
		// Set the Vertical Synchronization Active (VSA)
		DSI->VVSACR = VSA;
		// Set the Vertical Back Porch (VBP)
		DSI->VVBPCR = VBP;
		// Set the Vertical Front Porch (VFP)
		DSI->VVFPCR = VFP;
		// Set the Vertical Active period
		DSI->VVACR = VACT;
		// Low power largest packet size
		// Low power VACT largest packet size
		DSI->LPMCR = (64 << 16) | 64;
		// Configure the command transmission mode
		// Enable LP transition in HFP period
		// Enable LP transition in HBP period
		DSI->VMCR |= DSI_VMCR_LPCE | DSI_VMCR_LPHFPE | DSI_VMCR_LPHBPE | DSI_VMCR_LPVAE |
					 DSI_VMCR_LPVFPE | DSI_VMCR_LPVBPE | DSI_VMCR_LPVSAE;
	}
	{
		// Enable the DSI host
		DSI->CR = DSI_CR_EN;
		// Enable the DSI wrapper
		DSI->WCR = DSI_WCR_DSIEN;
	}

	{
		// LCD clock configuration
		// PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz
		// PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 384 Mhz
		// PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 384 MHz / 7 = 54.857 MHz
		// LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_2 = 54.857 MHz / 2 = 27.429 MHz
		RCC->PLLSAICFGR = (7 << 28) | (15 << 24) | (3 << 16) | (384 << 6);
		// Select PLLSAI clock for 48MHz clocks
		RCC->DCKCFGR = RCC_DCKCFGR_CK48MSEL;
		// Enable PLLSAI
		RCC->CR |= RCC_CR_PLLSAION;
		for (int t = 1024; not (RCC->CR & RCC_CR_PLLSAIRDY) and t; t--) {
			xpcc::delayMilliseconds(1);
		}
	}

	{
		// HAL_LTDC_Init(&hltdc_eval);
		// Configures the HS, VS, DE and PC polarity
		LTDC->GCR = 0;
		// Sets Synchronization size
		LTDC->SSCR = ((HSA - 1) << 16) | (VSA - 1);
		// Sets Accumulated Back porch
		LTDC->BPCR = ((HSA + HBP - 1) << 16) | (VSA + VBP - 1);
		// Sets Accumulated Active Width
		LTDC->AWCR = ((HACT + HSA + HBP - 1) << 16) | (VACT + VSA + VBP - 1);
		// Sets Total Width and Height
		LTDC->TWCR = ((HACT + HSA + HBP + HFP - 1) << 16) | (VACT + VSA + VBP + VFP - 1);
		// Sets the background color value
		LTDC->BCCR = 0;
		// Enable LTDC by setting LTDCEN bit
		LTDC->GCR |= LTDC_GCR_LTDCEN;
	}

	otm8009a_init(ColorCoding);

	{
		// HAL_LTDC_ConfigLayer()
		// Configures the horizontal start and stop position
		LTDC_Layer1->WHPCR = ((HACT + HSA + HBP - 1) << 16) | (HSA + HBP);
		// Configures the vertical start and stop position
		LTDC_Layer1->WVPCR  = ((VACT + VSA + VBP - 1) << 16) | (VSA + VBP);
		// Specifies the pixel format
		LTDC_Layer1->PFCR = ColorCoding;
		// Configures the default color values
		LTDC_Layer1->DCCR = 0xff000000;
		// Specifies the constant alpha value
		LTDC_Layer1->CACR = 0xff;
		// Specifies the blending factors
		LTDC_Layer1->BFCR = 0x607;
		// Configures the color frame buffer pitch in byte
		LTDC_Layer1->CFBLR = ((HACT * pixel_size) << 16) | ((HACT * pixel_size) + 3);
		// Configures the frame buffer line number
		LTDC_Layer1->CFBLNR = VACT;

		/* Configured in display.cpp
		// Configures the color frame buffer start address
		LTDC_Layer1->CFBAR = buffer_address;
		// Enable LTDC_Layer by setting LEN bit
		LTDC_Layer1->CR = LTDC_LxCR_LEN;
		// Sets the Reload type
		LTDC->SRCR = LTDC_SRCR_IMR;
		*/
	}
}
