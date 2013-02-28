#ifndef FSMC_HPP
#define FSMC_HPP

#include <stdint.h>

namespace xpcc
{
	namespace stm32
	{
		class FsmcDisplayS75
		{
		public:
			static void
			initialize();

			static void
			writeRegister(const uint8_t reg);

			static void
			writeCommand(const uint8_t command, const uint16_t data);

			static void
			writeDataMult(const uint16_t data, const uint16_t count);

			static void
			writeRam(const uint8_t * addr, const uint16_t size);

		protected:
			static void
			writeData(const uint16_t data);

			/**
			 * Accessing the same address twice will destroy the
			 * first write.
			 * When writing to an LC display with auto-incrementing
			 * memory pointer (and thus writing to the same address)
			 * it is necessary to access two different addresses
			 * of FSMC that will match to the same address in each write.
			 */
		protected:
			typedef class
			{
			public:
			  uint8_t REG1;
			private:
			  uint8_t dummy1[3];
			public:
//			  uint8_t REG2;
			  uint8_t dummy33;
			private:
			  uint8_t dummy2[(1 << 23) - 5];
			public:
			  uint8_t RAM1;
			private:
			  uint8_t dummy3[3];
			public:
//			  uint8_t RAM2;
			  uint8_t dummy99;
			} __attribute__((__packed__)) LCD_TypeDef;

			/* LCD is connected to the FSMC_Bank1_NOR/SRAM1 and NE1 is used as ship select signal */
			#define LCD_BASE    ((u32)(0x60000000 | 0x00000000))
			#define LCD         ((LCD_TypeDef *) LCD_BASE)
		};

	}
}

#endif // FSMC_HPP
