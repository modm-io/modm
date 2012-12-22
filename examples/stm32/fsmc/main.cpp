
#include <xpcc/architecture.hpp>
#include <xpcc/driver/ui/display/siemens_s75.hpp>

#include "stm32f4xx_fsmc.h"

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led, A, 8);
GPIO__INPUT(Button, C, 13);

using namespace xpcc::stm32;

namespace lcd
{
	GPIO__OUTPUT(D0,   D, 14);
	GPIO__OUTPUT(D1,   D, 15);
	GPIO__OUTPUT(D2,   D,  0);
	GPIO__OUTPUT(D3,   D,  1);
	GPIO__OUTPUT(D4,   E,  7);
	GPIO__OUTPUT(D5,   E,  8);
	GPIO__OUTPUT(D6,   E,  9);
	GPIO__OUTPUT(D7,   E, 10);

	typedef xpcc::gpio::Port<D7, D6, D5, D4, D3, D2, D1, D0> Port;

	GPIO__OUTPUT(Cs,    D,  7);		// Chip Select,     FSMC: NE1
	GPIO__OUTPUT(Cd,    E,  2);		// Command / Data,  FSMC: A23
	GPIO__OUTPUT(Wr,    D,  5);		// Write operation, FSMC: NWE

	GPIO__OUTPUT(Reset, E,  3);     // Reset, not FSMC

	typedef uint8_t Memory;
}

//typedef xpcc::SiemensS75Landscape<lcd::Port, lcd::Cs, lcd::Cd, lcd::Wr, lcd::Reset> Display;
typedef xpcc::SiemensS75Landscape<lcd::Memory, lcd::Reset> Display;

Display display;

// ----------------------------------------------------------------------------

/**
 * Pin out for FSMC,
 * 100 pin
 *
 * Signal  STM   Port      PCB    Display Pin
 * D0       61   PD14               6
 * D1       62   PD15              14
 * D2       81   PD 0              15
 * D3       82   PD 1              16
 * D4       38   PE 7              17
 * D5       39   PE 8              18
 * D6       40   PE 9              19
 * D7       41   PE10              20
 *
 * WR = NWE 86   PD 5              13
 * CS = NE1	88   PD 7               3
 * CD = A23  1   PE 2               1
 *
 * Reset         PE 3               2
 *
 */

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Switch STM32F4 to 168 MHz (HSE clocked by an 25 MHz external clock)
	if (Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		Clock::enablePll(Clock::PllSource::PLL_HSE, 25, 336);
		Clock::switchToPll();
	}
	
	Led::setOutput();
	Button::setInput(xpcc::stm32::PULLUP);
	

	// Init FSMC
	typedef class
	{
	public:
	  uint8_t LCD_REG;
	private:
	  uint8_t dummy[(1 << 23) - 1];
	public:
	  uint8_t LCD_RAM;
	} __attribute__((__packed__)) LCD_TypeDef;

	/* LCD is connected to the FSMC_Bank1_NOR/SRAM1 and NE1 is used as ship select signal */
	#define LCD_BASE    ((u32)(0x60000000 | 0x00000000))
	#define LCD         ((LCD_TypeDef *) LCD_BASE)

	// switch on FSMC peripheral
	#define RCC_AHB3Periph_FSMC               ((uint32_t)0x00000001)
	RCC->AHB3ENR |= RCC_AHB3Periph_FSMC;

	// Set pins to alternate function, push pull
	lcd::D0::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D1::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D2::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D3::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D4::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D5::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D6::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::D7::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);

	lcd::Cs::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::Cd::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	lcd::Wr::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);


	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;

	/*-- FSMC Configuration ------------------------------------------------------*/
	/* FSMC_Bank1_NORSRAM1 timing configuration */
	p.FSMC_AddressSetupTime = 0x0f;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 60;
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_B;

	//               Chip Select NE1
	//                   v
	/* FSMC_Bank1_NORSRAM1 configured as follows:
		- Data/Address MUX = Disable
		- Memory Type = SRAM
		- Data Width = 16bit
		- Write Operation = Enable
		- Extended Mode = Disable
		- Asynchronous Wait = Disable */
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);

	/* Enable FSMC_Bank1_NORSRAM1 */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);




	while(1)
	{
		LCD->LCD_REG = 0x55;   // A23 = 0
		LCD->LCD_RAM = 0xaa;   // A23 = 1

		Led::toggle();
		xpcc::delay_ms(50);
	}

	// -------------

	lcd::Port::setOutput();

	display.initialize();
	display.setFont(xpcc::font::Assertion);

	while (1) {
		static uint8_t y = 0;
		display.clear();
		display.setCursor(5, y);
		display << "Hello";
		display.setCursor(46, 16);
		display << "World!";

		// finished, copy to LCD
		display.update();

		Led::toggle();
		xpcc::delay_ms(50);

		if (++y > 10)
		{
			y = 0;
		}
	}
}
