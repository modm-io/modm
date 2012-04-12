/*
 * adc_1.hpp
 *
 *  Created on: 03.04.2012
 *      Author: dhebbeker
 */
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__ADC_HPP
#define XPCC_STM32__ADC_HPP

#include <xpcc/architecture.hpp>

namespace xpcc
{
	namespace stm32
	{	
		/**
		 * \brief	Analog/Digital-Converter module (ADC1)
		 *
		 * \author	Stephan Kugelmann, David Hebbeker
		 * \ingroup	stm32
		 */
		class Adc1 : public Interface
		{
		public:
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
		
				TEMPERATURE_SENSOR = 16,
				V_REFINT = 17,// internal reference voltage
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

			enum Prescaler
			{
				PRESCALER_2 = 0x00,
				PRESCALER_4 = ADC_CCR_ADCPRE_0,
				PRESCALER_6 = ADC_CCR_ADCPRE_1,
				PRESCALER_8 = ADC_CCR_ADCPRE_1 | ADC_CCR_ADCPRE_0
			};

			enum SampleTime
			{
				CYCLES_3 	= 0b000,
				CYCLES_15 	= 0b001,
				CYCLES_28 	= 0b010,
				CYCLES_56 	= 0b011,
				CYCLES_84 	= 0b100,
				CYCLES_112 	= 0b101,
				CYCLES_144 	= 0b110,
				CYCLES_480 	= 0b111
			};

			enum InterruptFlag
			{
				END_OF_CONVERSION_REGULAR	= ADC_SR_EOC,
				END_OF_CONVERSION_INJECTED	= ADC_SR_JEOC,
				ANALOG_WATCHDOG				= ADC_SR_AWD,
				OVERRUN						= ADC_SR_OVR
			};


		public:

			/**
			 * \brief Change the presentation of the ADC conversion result
			 *
			 * Set to \c true to left adjust the result. Otherwise, the result
			 * is right adjusted.
			 * Change will affect the ADC Data Register immediately, regardless
			 * of any ongoing conversions.
			 */
			static inline void
			setLeftAdjustResult(const bool enable);

			/**
			 * \brief Analog channel selection
			 */
			static inline void
			setChannel(const Channels channel, const SampleTime sampleTime=CYCLES_3)
			{
				ADC1->SQR1 = 0;		// clear number of conversions in the sequence and set number of conversions to 1
				ADC1->SQR3 |= channel;

				if(static_cast<uint8_t>(channel) < 10)
					ADC1->SMPR2 |= sampleTime << (static_cast<uint8_t>(channel) * 3);
				else
					ADC1->SMPR1 |= sampleTime << ((static_cast<uint8_t>(channel)-10) * 3);
					
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
			 */
			static inline void
			enableFreeRunningMode(void)
			{
				ADC1->CR2 |= ADC_CR2_CONT;		// set to continuous mode
			}

			static inline void
			disableFreeRunningMode(void)
			{
				ADC1->CR2 &= ~ADC_CR2_CONT;		// set to single
			}

			/**
			 * \return \c true if the flag is set
			 *
			 * Available on all ATmegas.
			 */
			static inline bool
			isInterruptFlagSet(const InterruptFlag flag)
			{
				return ADC1->SR & flag;
			}

			/**
			 * \brief Clears the interrupt flag
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			clearInterruptFlag(const InterruptFlag flag)
			{
				ADC1->SR &= ~flag;
			}


			static inline void
			disableInterrupt(void)
			{
				NVIC_DisableIRQ(ADC_IRQn);
				ADC1->CR1 &= ~ADC_CR1_EOCIE;
			}

			/**
			 * \brief Enables the ADC Conversion Complete Interrupt
			 *
			 * Available on all ATmegas.
			 *
			 * \see AdcInterrupt
			 */
			static inline void
			enableInterrupt(const uint8_t priority)
			{
				// Set priority for the interrupt vector
				NVIC_SetPriority(ADC_IRQn, priority);

				// register IRQ at the NVIC
				NVIC_EnableIRQ(ADC_IRQn);
				ADC1->CR1 |= ADC_CR1_EOCIE;
			}

			/**
			 * Set the division factor between the system clock frequency
			 * and the input clock to the ADC.
			 */
			static inline void
			setPrescaler(const Prescaler prescaler)
			{
				ADC->CCR &= 3 << 17;
				ADC->CCR |= prescaler << 17;
			}

			/**
			 * \brief	Initialize and enable the A/D converter
			 */
			static inline void
			initialize(Prescaler prescaler=PRESCALER_2)
			{
				// Initialize ADC
				RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // start ADC Clock

				ADC1->CR2 |= ADC_CR2_ADON;		// switch on ADC

				setPrescaler(prescaler);
			}

			static inline void
			shutdownADC(void)
			{
				ADC1->CR2 &= ~(ADC_CR2_ADON);
				RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN; // start ADC Clock
			}

			/**
			 * \brief Start a new conversion
			 */
			static inline void
			startConversion(void)
			{
				clearInterruptFlag(static_cast<InterruptFlag>(END_OF_CONVERSION_REGULAR | END_OF_CONVERSION_INJECTED | ANALOG_WATCHDOG | OVERRUN));
				ADC1->CR2 |= ADC_CR2_SWSTART;	// starts single conversion for the regular group
			}

			/**
			 * \brief Check if the conversion is finished
			 *
			 * Available on all ATmegas.
			 */
			static inline bool
			isConversionFinished(void)
			{
				return (ADC1->SR & ADC_SR_EOC);
			}

			/**
			 * \brief the most recent 16bit result of the ADC conversion
			 *
			 * Available on all ATmegas.
			 */
			static inline uint16_t
			getValue(void)
			{
				return ADC1->DR;
			}

		};
	}

}

#endif /* XPCC_STM32__ADC_HPP */