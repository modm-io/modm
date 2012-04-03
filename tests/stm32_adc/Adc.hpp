/*
 * Adc.hpp
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
		 * \brief	Generic Analog/Digital-Converter module
		 *
		 * \author	Stephan Kugelmann, David Hebbeker
		 * \ingroup	stm32
		 */
		class Adc : public Interface
		{
		public:
			enum Prescaler
			{
				PRESCALER_2 = 0x00,
				PRESCALER_4 = ADC_CCR_ADCPRE_0,
				PRESCALER_6 = ADC_CCR_ADCPRE_1,
				PRESCALER_8 = ADC_CCR_ADCPRE_1 | ADC_CCR_ADCPRE_0
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
			setLeftAdjustResult(bool enable);

			/**
			 * \brief Analog channel selection
			 *
			 * Selects which analog inputs are connected to the ADC.
			 * If this is changed during a conversion, the change will not go
			 * in effect until this conversion is complete.
			 *
			 * Available on all ATmegas.
			 */
			static inline bool
			setChannel(uint8_t channel);

			/**
			 * \return The analog channel connected to the ADC
			 *
			 * Available on all ATmegas.
			 */
			static inline uint8_t
			getChannel();

			/**
			 * \brief Enables the ADC
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			setEnableAdc(bool enable) {}

			/**
			 * In Single Conversion mode, this starts each conversion.
			 * In Free Running mode, this starts the first conversion.
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			setStartConversion() {}

			/**
			 * \brief Enable auto triggering of the ADC
			 *
			 * The ADC will start a conversion on a positive edge of the
			 * selected trigger signal.
			 * \see setAutoTriggerSource
			 */
			static inline void
			setAutoTriggerEnable(bool enable);

			/**
			 * \brief Enables free running mode
			 *
			 * The ADC will continously start conversions and provide the most
			 * recent result in the ADC register.
			 */
			static inline void
			setFreeRunningMode(bool enable);

			/**
			 * \return \c true if the flag is set
			 *
			 * Available on all ATmegas.
			 */
			static inline bool
			isInterruptFlagSet() {}

			/**
			 * \brief Clears the interrupt flag
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			clearInterruptFlag() {}

			/**
			 * \brief Enables the ADC Conversion Complete Interrupt
			 *
			 * Available on all ATmegas.
			 *
			 * \see AdcInterrupt
			 */
			static inline void
			setInterruptEnable(bool enable) {}

			/**
			 * Set the division factor between the system clock frequency
			 * and the input clock to the ADC.
			 */
			static inline void
			setPrescaler(Prescaler prescaler);


			/**
			 * \brief Selects which source will trigger an ADC conversion
			 *
			 * A conversion will be triggered by the rising edge of the
			 * selected Interrupt Flag. Note that switching from a trigger
			 * source that is cleared to a trigger source that is set, will
			 * generate a positive edge on the trigger signal.
			 * Set to 0 to enable Free Running Mode.
			 */
			static inline void
			setAutoTriggerSource(uint8_t source);

			static inline uint16_t
			getDataRegister();

			/**
			 * \brief	Initialize and enable the A/D converter
			 */
			static inline void
			initialize(Prescaler prescaler)
			{
				setPrescaler(prescaler);
				setEnableAdc(true);
			}

			/**
			 * \brief Correctly enables Free Running Mode
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			enableFreeRunningMode()
			{
				setFreeRunningMode(true);
			}

			/**
			 * \brief	Read the value an analog channel
			 */
			static inline uint16_t
			readChannel(uint8_t channel)
			{
				if (!startConversion(channel)) return 0;

				while (!isInterruptFlagSet()) {
					// wait until the conversion is finished
				}

				return getDataRegister();
			}

			/**
			 * \brief Start a new conversion
			 */
			static inline bool
			startConversion(uint8_t channel)
			{
				if (!setChannel(channel)) return false;

				clearInterruptFlag();
				setStartConversion();

				return true;
			}

			/**
			 * \brief Check if the conversion is finished
			 *
			 * Available on all ATmegas.
			 */
			static inline bool
			isFinished()
			{
				return isInterruptFlagSet();
			}

			/**
			 * \brief the most recent 16bit result of the ADC conversion
			 *
			 * Available on all ATmegas.
			 */
			static inline uint16_t
			getValue()
			{
				return getDataRegister();
			}

		};
	}

}

#endif /* XPCC_STM32__ADC_HPP */
