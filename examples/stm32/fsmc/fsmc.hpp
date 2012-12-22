#ifndef FSMC_HPP
#define FSMC_HPP

#include <stdint.h>

//	while(1)
//	{
//		LCD->LCD_REG = 0x55;   // A23 = 0
//		LCD->LCD_RAM = 0xaa;   // A23 = 1
//
//		Led::toggle();
//		xpcc::delay_ms(50);
//	}


namespace xpcc
{
	namespace stm32
	{
		class FsmcExperimental
		{
		public:
			static void
			initialize();

			static void
			writeRegister(uint8_t reg);

			static void
			writeCommand(uint8_t command, uint16_t data);

			static void
			writeDataMult(uint16_t data, uint16_t count);

			static void
			writeRam(uint8_t * addr, uint16_t size);

//		protected:
			static void
			writeData(uint16_t data);

			/**
			 * Accessing the same address twice will destroy the
			 * first write.
			 * When writing to a LC display with auto-incrementing
			 * memory pointer it is necessary to access two different
			 * addresses in each write.
			 */
		protected:
			typedef class
			{
			public:
			  uint8_t REG1;
			private:
			  uint8_t dummy1[3];
			public:
			  uint8_t REG2;
			private:
			  uint8_t dummy2[(1 << 23) - 5];
			public:
			  uint8_t RAM1;
			private:
			  uint8_t dummy3[3];
			public:
			  uint8_t RAM2;
			} __attribute__((__packed__)) LCD_TypeDef;

			/* LCD is connected to the FSMC_Bank1_NOR/SRAM1 and NE1 is used as ship select signal */
			#define LCD_BASE    ((u32)(0x60000000 | 0x00000000))
			#define LCD         ((LCD_TypeDef *) LCD_BASE)


		};

	}
}

#endif // FSMC_HPP
