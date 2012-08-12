#ifndef XPCC_LPC111X__ADC_HPP
#define XPCC_LPC111X__ADC_HPP

#include <xpcc/architecture.hpp>

namespace xpcc {
	namespace lpc111x {
		/**
		 * \brief 	Analog-to-Digital Converter Module of
		 * 			LPC111x, LPC11D14 and LPC11Cxx parts.
		 *
		 */
		class Adc
		{
		public:

			/**
			 * \brief	Channels which can be used as ADC input.
			 *
			 * You can specify the channel by using a pin-name, like PIO0_11
			 *  or just the plain channel number, like CHANNEL_0.
			 *
			 */
			enum class Channels
			{
				PIO0_11	= 0x01,
				PIO1_0 	= 0x02,
				PIO1_1	= 0x04,
				PIO1_2	= 0x08,
//				PIO1_3	= 0x10,
				PIO1_4	= 0x20,
				PIO1_10	= 0x40,
				PIO1_11	= 0x80,

				CHANNEL_0 = 0x01,
				CHANNEL_1 = 0x02,
				CHANNEL_2 = 0x04,
				CHANNEL_3 = 0x08,
//				CHANNEL_4 = 0x10,
				CHANNEL_5 = 0x20,
				CHANNEL_6 = 0x40,
				CHANNEL_7 = 0x80,
			};

			enum class Resolution
			{
				BITS_10 = (0x0 << 17),	///< 11 clocks / 10 bits
				BITS_9 	= (0x1 << 17),	///< 10 clocks / 9 bits
				BITS_8 	= (0x2 << 17),	///< 9 clocks / 8 bits
				BITS_7 	= (0x3 << 17),	///< 8 clocks / 7 bits
				BITS_6 	= (0x4 << 17),	///< 7 clocks / 6 bits
				BITS_5 	= (0x5 << 17),	///< 6 clocks / 5 bits
				BITS_4 	= (0x6 << 17),	///< 5 clocks / 4 bits
				BITS_3 	= (0x7 << 17),	///< 4 clocks / 3 bits
			};

			static void inline
			initialize(uint8_t channelBitmask)
			{
				  /* Disable Power down bit to the ADC block. */
				  LPC_SYSCON->PDRUNCFG &= ~(0x1<<4);

				  /* Enable AHB clock to the ADC. */
				  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<13);

				  if (channelBitmask & 0x01) {
					  LPC_IOCON->R_PIO0_11 &= ~0x8F; /*  ADC I/O config */
					  LPC_IOCON->R_PIO0_11 |= 0x02;  /* ADC IN0 */
				  }
				  if (channelBitmask & 0x02) {
					  LPC_IOCON->R_PIO1_0  &= ~0x8F;
					  LPC_IOCON->R_PIO1_0  |= 0x02;  /* ADC IN1 */
				  }
				  if (channelBitmask & 0x04) {
					  LPC_IOCON->R_PIO1_1  &= ~0x8F;
					  LPC_IOCON->R_PIO1_1  |= 0x02;  /* ADC IN2 */
				  }
				  if (channelBitmask & 0x08) {
					  LPC_IOCON->R_PIO1_2 &= ~0x8F;
					  LPC_IOCON->R_PIO1_2 |= 0x02;	/* ADC IN3 */
				  }
//				  if (channelBitmask & 0x10) {
//
//				  }
				  if (channelBitmask & 0x20) {
					  LPC_IOCON->PIO1_4    = 0x01;	// Select AD5 pin function
				  }
				  if (channelBitmask & 0x40) {
					  LPC_IOCON->PIO1_10   = 0x01;	// Select AD6 pin function
				  }
				  if (channelBitmask & 0x80) {
					  LPC_IOCON->PIO1_11   = 0x01;	// Select AD7 pin function
				  }

				  /* Set clock */
				  LPC_ADC->CR = 10; // 48 MHz / 11 = 4.35 MHz

			}

		protected:
			static constexpr uint8_t
			adcCount = 8;
		};


	}  // namespace lpc111x
}

#endif // XPCC_LPC111X__ADC_HPP
