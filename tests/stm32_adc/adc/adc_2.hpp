/*
 * adc_2.hpp
 *
 *  Created on: 03.04.2012
 *      Author: dhebbeker
 */

#ifndef XPCC_STM32__ADC_HPP
#define XPCC_STM32__ADC_HPP

#include <xpcc/architecture.hpp>

namespace xpcc
{
	namespace stm32
	{
		/**
		 * Analog/Digital-Converter module (ADC2).
		 *
		 * \author	Stephan Kugelmann, David Hebbeker
		 * \ingroup	stm32
		 */
		class Adc2 : public Interface
		{
		public:
			
			/**
			 * Channels, which can be used with this ADC.
			 * 
			 * You can specify the channel by using a pin-name, like PIN_C0, an 
			 * internal sensor, like TEMPERATURE_SENSOR or just the plain 
			 * channel number, like CHANNEL_0. 
			 */
			enum Channels
			{
				PIN_C0 = 10,
				PIN_C1 = 11,
				PIN_C2 = 12,
				PIN_C3 = 13,
				PIN_A0 = 0,
				PIN_A1 = 1,
				PIN_A2 = 2,
				PIN_A3 = 3,
				PIN_A4 = 4,
				PIN_A5 = 5,
				PIN_A6 = 6,
				PIN_A7 = 7,
				PIN_C4 = 14,
				PIN_C5 = 15,
				PIN_B0 = 8,
				PIN_B1 = 9,
				/** Measure the ambient temperature of the device.
				 * 
				 * \li Supported temperature range: -40 to 125 C
				 * \li Precision: +-1.5 C
				 * 
				 * @see Reference manual (i.e. RM0090) for the formula for the
				 * 	calculation of the actual temperature.
				 * @note The TSVREFE bit must be set to enable the conversion of 
				 * 	this internal channel.
				 */
				TEMPERATURE_SENSOR = 16,

				/** Internal reference voltage.
				 * 
				 * @note The TSVREFE bit must be set to enable the conversion of 
				 * 	this internal channel.
				 */
				V_REFINT = 17,
				
				/**
				 * The half V_BAT voltage.
				 */
				VBAT = 18,

				CHANNEL_0 = 0,
				CHANNEL_1 = 1,
				CHANNEL_2 = 2,
				CHANNEL_3 = 3,
				CHANNEL_4 = 4,
				CHANNEL_5 = 5,
				CHANNEL_6 = 6,
				CHANNEL_7 = 7,
				CHANNEL_8 = 8,
				CHANNEL_9 = 9,
				CHANNEL_10= 10,
				CHANNEL_11 = 11,
				CHANNEL_12 = 12,
				CHANNEL_13 = 13,
				CHANNEL_14 = 14,
				CHANNEL_15 = 15,
				CHANNEL_16 = 16,
				CHANNEL_17 = 17,
				CHANNEL_18 = 18
			};
			
			/**
			 * Programmable prescaler to divide the APB2 clock frequency, which 
			 * is used fot the analog circuitry (not the digital interface which
			 * is used for registers). 
			 */
			enum Prescaler
			{
				PRESCALER_2 = 0x00,									//!< PCLK2 divided by 2
				PRESCALER_4 = ADC_CCR_ADCPRE_0,						//!< PCLK2 divided by 4
				PRESCALER_6 = ADC_CCR_ADCPRE_1,						//!< PCLK2 divided by 6
				PRESCALER_8 = ADC_CCR_ADCPRE_1 | ADC_CCR_ADCPRE_0	//!< PCLK2 divided by 8
			};
			
			/**
			 * Sampling time of the input voltage.
			 * 
			 * Total conversion time is T_con = Sampling time + 12 cycles
			 */
			enum SampleTime
			{
				CYCLES_3 	= 0b000,	//!< 3 ADCCLK cycles
				CYCLES_15 	= 0b001,	//!< 15 ADCCLK cycles
				CYCLES_28 	= 0b010,	//!< 28 ADCCLK cycles
				CYCLES_56 	= 0b011,	//!< 56 ADCCLK cycles
				CYCLES_84 	= 0b100,	//!< 84 ADCCLK cycles
				CYCLES_112 	= 0b101,	//!< 112 ADCCLK cycles
				CYCLES_144 	= 0b110,	//!< 144 ADCCLK cycles
				CYCLES_480 	= 0b111		//!< 480 ADCCLK cycles
			}; 

			/**
			 * Possible interrupt flags.
			 * 
			 * An interrupt can be produced on the end of conversion for regular
			 * and injected groups, when the analog watchdog status bit is set 
			 * and when the overrun status bit is set. 
			 */
			enum InterruptFlag
			{
				END_OF_CONVERSION_REGULAR	= ADC_SR_EOC,	//!< End of conversion of a regular group
				END_OF_CONVERSION_INJECTED	= ADC_SR_JEOC,	//!< End of conversion of an injected group
				ANALOG_WATCHDOG				= ADC_SR_AWD,	//!< Analog watchdog status bit is set 
				OVERRUN						= ADC_SR_OVR	//!< Overrun (if data are lost)
			};


		public:

			/**
			 * Change the presentation of the ADC conversion result.
			 *
			 * @param enable Set to \c true to left adjust the result. 
			 * 	Otherwise, the result is right adjusted.
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 */
			static inline void
			setLeftAdjustResult(const bool enable)
			{
				if(enable)
					ADC2->CR2 |= ADC_CR2_ALIGN;
				else
					ADC2->CR2 &= ~ADC_CR2_ALIGN;
						
			}

			/**
			 * Analog channel selection.
			 * 
			 * This is for single conversion mode only! The number of channels
			 * will be set to 1, the channel selected and the corresponding pin
			 * will be set to analog input.
			 * 
			 * @param channel		The channel which shall be read.
			 * @param sampleTime	The sample time to sample the input voltage.
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 */
			static inline void
			setChannel(const Channels channel, const SampleTime sampleTime=CYCLES_3)
			{
				ADC2->SQR1 = 0;		// clear number of conversions in the sequence and set number of conversions to 1
				ADC2->SQR3 |= channel;

				if(static_cast<uint8_t>(channel) < 10)
					ADC2->SMPR2 |= sampleTime << (static_cast<uint8_t>(channel) * 3);
				else
					ADC2->SMPR1 |= sampleTime << ((static_cast<uint8_t>(channel)-10) * 3);

				if(channel <8)
					GPIOA->MODER |= 0b11 << ((channel + 0) * 2);
				else if(channel <10)
					GPIOB->MODER |= 0b11 << ((channel - 8) * 2);
				else if(channel <16)
					GPIOC->MODER |= 0b11 << ((channel - 10) * 2);
				}

			/**
			 * \brief Enables free running mode
			 *
			 * The ADC will continously start conversions and provide the most
			 * recent result in the ADC register.
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 */
			static inline void
			enableFreeRunningMode(void)
			{
				ADC2->CR2 |= ADC_CR2_CONT;		// set to continuous mode
			}

			/**
			 * \brief Disables free running mode
			 *
			 * The ADC will do only one sample and stop. The result will be in 
			 * the ADC register.
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 */
			static inline void
			disableFreeRunningMode(void)
			{
				ADC2->CR2 &= ~ADC_CR2_CONT;		// set to single
			}

			/**
			 * Returns if the specified interrupt flag is set.
			 * 
			 * \return \c true if the flag is set
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 * @param flag The interrupt flag, which shall be checked.
			 */
			static inline bool
			isInterruptFlagSet(const InterruptFlag flag)
			{
				return ADC2->SR & flag;
			}

			/**
			 * Clears the specified interrupt flag.
			 *
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 * @param flag The interrupt flag, which shall be cleared.
			 */
			static inline void
			clearInterruptFlag(const InterruptFlag flag)
			{
				ADC2->SR &= ~flag;
			}

			/**
			 * Disables the ADC Conversion Complete Interrupt.
			 */
			static inline void
			disableInterrupt(void)
			{
				NVIC_DisableIRQ(ADC_IRQn);
				ADC2->CR1 &= ~ADC_CR1_EOCIE;
			}

			/**
			 * Enables the ADC Conversion Complete Interrupt.
			 *
			 * You could catch the interrupt using this function:
			 * 	\code extern "C" void ADC_IRQHandler(void) \endcode
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 * @param priority Priority to set
			 */
			static inline void
			enableInterrupt(const uint32_t priority)
			{
				// Set priority for the interrupt vector
				NVIC_SetPriority(ADC_IRQn, priority);

				// register IRQ at the NVIC
				NVIC_EnableIRQ(ADC_IRQn);
				ADC2->CR1 |= ADC_CR1_EOCIE;
			}

			/** 
			 * Select the frequency of the clock to the ADC. The clock is common
			 * for all the ADCs (ADC1, ADC2, ADC3) and all channels. 
			 * @pre The ADC clock must be started and the ADC switched on with initialize()
			 * @param prescaler The prescaler specifies by which factor the 
			 * 	system clock will be divided.
			 */
			static inline void
			setPrescaler(const Prescaler prescaler)
			{
				ADC->CCR &= 3 << 17;
				ADC->CCR |= prescaler << 17;
			}

			/**
			 * Initialize and enable the A/D converter.
			 *
			 * Enables the ADC clock and switches on the ADC. The ADC clock
			 * prescaler will be set as well.
			 * @param prescaler The prescaler specifies by which factor the 
			 * 	system clock will be divided.
			 */
			static inline void
			initialize(Prescaler prescaler=PRESCALER_2)
			{
				// Initialize ADC
				RCC->APB2ENR |= RCC_APB2ENR_ADC2EN; // start ADC Clock

				ADC2->CR2 |= ADC_CR2_ADON;		// switch on ADC

				setPrescaler(prescaler);
			}

			/**
			 * Turns off the ADC and its clock.
			 */
			static inline void
			shutdownADC(void)
			{
				ADC2->CR2 &= ~(ADC_CR2_ADON);
				RCC->APB2ENR &= ~RCC_APB2ENR_ADC2EN; // start ADC Clock
			}

			/**
			 * Start a new conversion or continuous conversions.
			 * @pre A ADC channel must be selected with setChannel()
			 * @post The result can be fetched with getValue()
			 */
			static inline void
			startConversion(void)
			{
				clearInterruptFlag(static_cast<InterruptFlag>(END_OF_CONVERSION_REGULAR | END_OF_CONVERSION_INJECTED | ANALOG_WATCHDOG | OVERRUN));
				ADC2->CR2 |= ADC_CR2_SWSTART;	// starts single conversion for the regular group
			}

			/** 
			 * @return If the conversion is finished.
			 * @pre A conversion should have been stared with startConversion()
			 */
			static inline bool
			isConversionFinished(void)
			{
				return (ADC2->SR & ADC_SR_EOC);
			}

			/**
			 * @return The most recent 16bit result of the ADC conversion.
			 * @pre A conversion should have been stared with startConversion()
			 */
			static inline uint16_t
			getValue(void)
			{
				return ADC2->DR;
			}

		};
	}

}

#endif /* XPCC_STM32__ADC_HPP */