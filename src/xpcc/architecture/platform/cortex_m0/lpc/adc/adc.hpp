#ifndef XPCC_LPC111X__ADC_HPP
#define XPCC_LPC111X__ADC_HPP

#include <xpcc/architecture.hpp>

#define ADC_GDR_DONE (1 << 31)
#define ADC_DR_DONE ADC_GDR_DONE

/* ---------- ADC Control Register bit names ------------------*/
#define ADC_CR_SEL_MASK				(0xff << 0)	///< Mask for deleting Channel selection
#define ADC_CR_BURST				(  1 << 16)	///< Burst mode
#define ADC_CR_START_NOW			(0x1 << 24)	///< Manually start a conversion now
#define ADC_CR_START_PIO0_2			(0x2 << 24)	///< Start conversion whenever the selected edge on PIO0_2 occurs
#define ADC_CR_START_PIO1_5			(0x3 << 24)	///< Start conversion whenever the selected edge on PIO1_5 occurs
#define ADC_CR_START_CT32B0_MAT0	(0x4 << 24)	///< Start conversion whenever the selected edge on CT32B0_MAT0 occurs (timer match)
#define ADC_CR_START_CT32B0_MAT1	(0x5 << 24)	///< Start conversion whenever the selected edge on CT32B0_MAT1 occurs (timer match)
#define ADC_CR_START_CT16B0_MAT0	(0x6 << 24)	///< Start conversion whenever the selected edge on CT16B0_MAT0 occurs (timer match)
#define ADC_CR_START_CT16B0_MAT1	(0x7 << 24)	///< Start conversion whenever the selected edge on CT16B0_MAT1 occurs (timer match)
#define ADC_CR_EDGE_RISING			(  0 << 27)	///< Start conversion whenever a rising edge occurs
#define ADC_CR_EDGE_FALLING			(  1 << 27)	///< Start conversion whenever a falling edge occurs
#define ADC_CR_START_EDGE_MASK			ADC_CR_START_CT16B0_MAT1 | ADC_CR_EDGE_FALLING

#define ADC_INTEN_ADGINTEN			(  1 << 8)	///< Interrupt when global DONE is 1

/* ---------- Power-down configuration register bit names -----*/
#define PDRUNCFG_ADC_PD (1 << 4)

/* ---------- System AHB clock control register bit names -----*/
#define SYSAHBCLKCTRL_ADC (1 << 13)

#define ADC_OFFSET		0x10
#define ADC_INDEX		4

namespace xpcc {
	namespace lpc111x {
		/**
		 * \brief 	Analog-to-Digital Converter Module of
		 * 			LPC111x, LPC11D14 and LPC11Cxx parts.
		 *
		 * Two usage scenarios where considered when designing this class:
		 *
		 * 1) Manual control: Single Channel, Single Shot
		 *    From a given set of channels a conversion can be started
		 *    manually and the result is fetched after waiting for the
		 *    ADC to finish this single channel. The ADC stops after
		 *    this sample automatically.
		 *    When using timers or external pins as a start condition repeated
		 *    conversion can be achieved.
		 *
		 * 2) Automatic mode: Multiple Channels, automatic repeat
		 *    Some channels are selected to be sampled automatically.
		 *    The latest result can be fetched by polling or an interrupt
		 *    handler can be installed.
		 *
		 * Whats not possible by hardware: convert multiple channels
		 * triggered by a timer. When triggered by a timer only a single
		 * channel (BURST = 0) can be selected.
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
			enum class ChannelMask
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

			enum class Channel
			{
				PIO0_11	= 0,
				PIO1_0 	= 1,
				PIO1_1	= 2,
				PIO1_2	= 3,
//				PIO1_3	= 4,
				PIO1_4	= 5,
				PIO1_10	= 6,
				PIO1_11	= 7,

				CHANNEL_0 = 0,
				CHANNEL_1 = 1,
				CHANNEL_2 = 2,
				CHANNEL_3 = 3,
//				CHANNEL_4 = 4,
				CHANNEL_5 = 5,
				CHANNEL_6 = 6,
				CHANNEL_7 = 7,
			};

			/**
			 * \brief	Configure the selected channels as analog input.
			 *
			 * \param	channelBitmask	Select which channels are configured
			 * 							by a bitmask. The corresponding IO pins
			 * 							are changed to analog mode.
			 */
			static void inline
			configurePins(uint8_t channelBitmask = 0xff)
			{
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
//				  if (channelBitmask & 0x10) {}
				  if (channelBitmask & 0x20) {
					  LPC_IOCON->PIO1_4    = 0x01;	// Select AD5 pin function
				  }
				  if (channelBitmask & 0x40) {
					  LPC_IOCON->PIO1_10   = 0x01;	// Select AD6 pin function
				  }
				  if (channelBitmask & 0x80) {
					  LPC_IOCON->PIO1_11   = 0x01;	// Select AD7 pin function
				  }
			}

			/**
			 * \brief	Initialise the ADC block.
			 *
			 * \param	mode			Software controlled start or free running mode.
			 */
			static void inline
			initialize()
			{
				  /* Disable Power down bit to the ADC block. */
				  LPC_SYSCON->PDRUNCFG &= ~(PDRUNCFG_ADC_PD);

				  /* Enable AHB clock to the ADC. */
				  LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_ADC;

				  /* Set clock */
				  LPC_ADC->CR = (10 << 8);
			}
		};

		/**
		 * \brief	Implementation of first usage scenario of ADC.
		 *
		 * Use all the common features from Adc class.
		 */
		class AdcManualSingle : public Adc
		{
		public:
			/**
			 * \brief	Start condition of the ADC.
			 *
			 * Only in software controlled mode a start condition can be given.
			 * If the conversion is started manually with START_NOW only one
			 * conversion is made.
			 * If a hardware source as a start condition is selected repeated
			 * samples can be generated if this condition appears regularly
			 * (e.g. repeated timer match)
			 */
			enum class StartCondition
			{
				START_NOW 			= ADC_CR_START_NOW,			///< Manually start a conversion now
				START_PIO0_2 		= ADC_CR_START_PIO0_2,		///< Start conversion whenever the selected edge on PIO0_2 occurs
				START_PIO1_5 		= ADC_CR_START_PIO1_5,		///< Start conversion whenever the selected edge on PIO1_5 occurs
				START_CT32B0_MAT0	= ADC_CR_START_CT32B0_MAT0, ///< Start conversion whenever the selected edge on CT32B0_MAT0 occurs (timer match)
				START_CT32B0_MAT1	= ADC_CR_START_CT32B0_MAT1, ///< Start conversion whenever the selected edge on CT32B0_MAT1 occurs (timer match)
				START_CT16B0_MAT0	= ADC_CR_START_CT16B0_MAT0, ///< Start conversion whenever the selected edge on CT16B0_MAT0 occurs (timer match)
				START_CT16B0_MAT1	= ADC_CR_START_CT16B0_MAT1, ///< Start conversion whenever the selected edge on CT16B0_MAT1 occurs (timer match)
			};

			enum class StartEdge
			{
				RISING 	= ADC_CR_EDGE_RISING,
				FALLING	= ADC_CR_EDGE_FALLING,
			};

			/**
			 * \brief	Start a single conversion of the single selected channel.
			 */
			static inline void
			startConversion(
					ChannelMask channelMask,
					StartCondition startCondition = StartCondition::START_NOW,
					StartEdge startEdge = StartEdge::RISING)
			{
				// clear and then select channel bits
				LPC_ADC->CR &= ~(ADC_CR_SEL_MASK | ADC_CR_START_EDGE_MASK);
				LPC_ADC->CR |=
						static_cast<uint32_t>(startCondition) |
						static_cast<uint32_t>(startEdge) |
						static_cast<uint32_t>(channelMask);
			}

			/**
			 * \brief	Check if the conversion is finished.
			 */
			static inline bool
			isConversionFinished(void)
			{
				return (LPC_ADC->GDR & ADC_GDR_DONE);
			}

			/**
			 * \brief	Get the latest value from the ADC
			 */
			static inline uint16_t
			getValue()
			{
				// Result is left adjuste to a 16 bit boundary
				// Convert to right adjusted value.
				return ((LPC_ADC->GDR & 0xffff) >> 6);
			}
		};

		/**
		 * \brief	Converting multiple channels in free running mode
		 */
		class AdcAutomaticBurst : public Adc
		{
		public:

			/**
			 * \brief	Resolution of the successive-approximation ADC.
			 */
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

			/**
			 * \brief
			 *
			 * \param	resolution		More bits mean lower conversion rate.
			 * 							Only meaningful in burst mode.
			 */
			static inline void
			initialize (Resolution resolution = Resolution::BITS_10)
			{
			  /* Disable Power down bit to the ADC block. */
			  LPC_SYSCON->PDRUNCFG &= ~(PDRUNCFG_ADC_PD);

			  /* Enable AHB clock to the ADC. */
			  LPC_SYSCON->SYSAHBCLKCTRL |= SYSAHBCLKCTRL_ADC;

			  /* Disable ADGINTEN in INTEN */
			  LPC_ADC->INTEN &= ~ADC_INTEN_ADGINTEN;

			  /* Set clock and resolution */
			  LPC_ADC->CR = (static_cast<uint32_t>(resolution)) | (10 << 8);
			}

			/**
			 * \brief	Start a conversion of the selected channel(s).
			 *
			 * The conversion is repeated automatically.
			 * When using interrupts it may be a good idea to set only on bit in the
			 * interruptMask to generate an interrupt when all channels are converted once.
			 *
			 * \param	channelMask		Bitmask of the channels that should be converted
			 * \param	interruptMask	Bitmask of channels that will generate an interrupt.
			 */
			static inline void
			startConversion(uint8_t channelMask, uint8_t interruptMask = 0)
			{
				// clear and then set the interrupt Mask, ADGINEN is cleared, too.
				LPC_ADC->INTEN = interruptMask;

				// clear and then select channel bits
				LPC_ADC->CR &= ~(ADC_CR_SEL_MASK | ADC_CR_START_EDGE_MASK);
				LPC_ADC->CR |= channelMask;

				// Set burst to start conversion
				LPC_ADC->CR |= ADC_CR_BURST;
			}

			/**
			 * \brief	Check if a single channel has finished
			 */
			static inline bool
			isConversionFinished(Channel channel)
			{
				return (getAdcRegister(channel) & ADC_DR_DONE);
			}

			/**
			 * \brief	Check if a group of channels have finished.
			 *
			 * Can be used for a single channel too if the ChannelMask is known.
			 */
			static inline bool
			isConversionFinished(uint8_t channelMask)
			{
				return ((LPC_ADC->STAT & 0xff) & channelMask);
			}

			/**
			 * \brief	Get the value of a single channel. Check before if result is ready.
			 */
			static inline uint16_t
			getValue(Channel channel)
			{
				return ((getAdcRegister(channel) & 0xffff) >> 6);
			}

			/**
			 * \brief	Returns true if the value is ready and puts it into the variable.
			 */
			static inline bool
			read(uint16_t & val, Channel channel)
			{
				uint32_t reg = getAdcRegister(channel);
				if (reg & ADC_DR_DONE) {
					val = (reg & 0xffff) >> 6;
					return true;
				}
				else {
					return false;
				}
			}

		protected:
			static inline uint32_t
			getAdcRegister(Channel channel)
			{
				return (*(volatile unsigned long *)
						(LPC_ADC_BASE + ADC_OFFSET + ADC_INDEX * static_cast<uint8_t>(channel)));
			}

		};


	}  // namespace lpc111x
}

#endif // XPCC_LPC111X__ADC_HPP
