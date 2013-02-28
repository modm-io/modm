#include "fsmc.hpp"

#include <xpcc/architecture.hpp>

#include "stm32f4xx_fsmc.h"

GPIO__OUTPUT(D0,   D, 14);
GPIO__OUTPUT(D1,   D, 15);
GPIO__OUTPUT(D2,   D,  0);
GPIO__OUTPUT(D3,   D,  1);
GPIO__OUTPUT(D4,   E,  7);
GPIO__OUTPUT(D5,   E,  8);
GPIO__OUTPUT(D6,   E,  9);
GPIO__OUTPUT(D7,   E, 10);

GPIO__OUTPUT(Cs,    D,  7);		// Chip Select,     FSMC: NE1
GPIO__OUTPUT(Cd,    E,  2);		// Command / Data,  FSMC: A23
GPIO__OUTPUT(Wr,    D,  5);		// Write operation, FSMC: NWE

void
xpcc::stm32::FsmcDisplayS75::initialize()
{
	// switch on FSMC peripheral
	#define RCC_AHB3Periph_FSMC               ((uint32_t)0x00000001)
	RCC->AHB3ENR |= RCC_AHB3Periph_FSMC;

	// Set pins to alternate function, push pull
	D0::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D1::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D2::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D3::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D4::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D5::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D6::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	D7::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);

	Cs::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	Cd::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);
	Wr::setAlternateFunction(xpcc::stm32::AF_FSMC, xpcc::stm32::PUSH_PULL, xpcc::stm32::SPEED_50MHZ, xpcc::stm32::PULLUP);

	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;

	/*-- FSMC Configuration ------------------------------------------------------*/
	/* FSMC_Bank1_NORSRAM1 timing configuration */
	p.FSMC_AddressSetupTime = 0x0f;
	p.FSMC_AddressHoldTime = 0;
	p.FSMC_DataSetupTime = 20;
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
}

void
xpcc::stm32::FsmcDisplayS75::writeCommand(const uint8_t command, const uint16_t data)
{
	writeRegister(command);
	writeData(data);
}

void
xpcc::stm32::FsmcDisplayS75::writeDataMult(const uint16_t data, const uint16_t count)
{
	for (uint_fast16_t ii = count; ii > 0; --ii)
	{
		LCD->RAM1 = data >> 8;
		LCD->RAM1 = data;
	}
}

void
xpcc::stm32::FsmcDisplayS75::writeRam(const uint8_t * addr, const uint16_t size)
{
	for (uint_fast16_t ii = size; ii > 0; --ii)
	{
		LCD->RAM1 = *(addr++);
		if (--ii == 0)
		{
			return;
		}
		LCD->RAM1 = *(addr++);
	}
}

void
xpcc::stm32::FsmcDisplayS75::writeRegister(const uint8_t reg)
{
	LCD->REG1 = 0;
	LCD->REG1 = reg;
}

void
xpcc::stm32::FsmcDisplayS75::writeData(const uint16_t data)
{
	LCD->RAM1 = data >> 8;
	LCD->RAM1 = data;
}
