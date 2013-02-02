// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATMEGA__ADC_HPP
#define XPCC_ATMEGA__ADC_HPP

#include <avr/io.h>

#if defined (__AVR_ATmega64HVE__)
	#error "The ATmega64HVE is not supported by this ADC class."
#endif
#if defined ADCSR
	#undef ADCSRA
	#define ADCSRA ADCSR
#endif
#if defined VADMUX
	#undef ADMUX
	#define ADMUX VADMUX
#endif

namespace xpcc
{
	namespace atmega
	{
		/**
		 * \brief	Generic Analog/Digital-Converter module
		 * 
		 * This class aims at providing a common interface to all the different
		 * register layouts of the ADC modules in most ATmegas.
		 * It takes into consideration restrictions and extensions in ADC
		 * functionality and provides the appropriate methods to configure them.
		 * 
		 * This class enables you to address and use a broader array of ATmegas
		 * with similar ADC functionality without changing a single line of code.
		 * 
		 * For best use of this class, check your device's datasheet for the
		 * supported functionality.
		 *
		 * ADC clock frequency should be between 50 and 200 kHz for maximum
		 * resolution. If less than 10-bits are needed the frequency can be higher.
		 * 
		 * Synchronous mode (waits until the ADC is finished):
		 * \code
		 * #include <xpcc/architecture/atmega/adc.hpp>
		 * 
		 * int
		 * main()
		 * {
		 *     xpcc::atmega::Adc::initialize(
		 *             xpcc::atmega:Adc::REFERENCE_INTERNAL_2V56,
		 *             xpcc::atmega:Adc::PRESCALER_64);
		 *     
		 *     ...
		 *     // read channel 2
		 *     uint16_t value = xpcc::atmega:Adc::readChannel(2);
		 *     ...
		 * }
		 * \endcode
		 * 
		 * Asynchronous mode:
		 * \code
		 * // start a new conversion on channel 3
		 * xpcc::atmega:Adc::startConversion(3);
		 * 
		 * // do something useful while waiting for the result 
		 * while (!xpcc::atmega:Adc::isFinished())
		 *     ;
		 * 
		 * // read the converted value
		 * uint16_t value = xpcc::atmega:Adc::getValue();
		 * \endcode
		 *
		 * Alternatively you can use the AdcInterrupt class to attach a function
		 * to the ADC Interrupt handler for true asynchonous mode.
		 * 
		 * For a detailed example see the \c adc folder in the examples folder.
		 * 
		 * \see AdcInterrupt
		 * \author	Fabian Greif, Niklas Hauser
		 * \ingroup	atmega
		 */
		class Adc
		{
		public:
			enum Reference
			{
#if defined ISRCEN
				// REF1 REF0 AREFEN -
				REFERENCE_AREF = 0x20, //0010
				REFERENCE_INTERNAL_AVCC = 0x60, //0110
				REFERENCE_INTERNAL_AVCC_NO_CAP = 0x40, //0100
				REFERENCE_INTERNAL_2V56 = 0xe0, //1110
				REFERENCE_INTERNAL_2V56_NO_CAP = 0xc0 //1100
#elif defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega164A__) || defined (__AVR_ATmega164P__) || defined (__AVR_ATmega324A__) || defined (__AVR_ATmega324P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega644A__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
				REFERENCE_AREF = 0,
				REFERENCE_INTERNAL_AVCC = 0x40,
				REFERENCE_INTERNAL_1V1 = 0x80,
				REFERENCE_INTERNAL_2V56 = 0xc0
#elif defined (__AVR_ATmega128RFA1__)
				REFERENCE_AREF = 0,
				REFERENCE_INTERNAL_AVDD_NO_CAP = 0x40,
				REFERENCE_INTERNAL_1V5_NO_CAP = 0x80,
				REFERENCE_INTERNAL_1V6_NO_CAP = 0xc0
#elif defined (__AVR_ATmega128__) || defined (__AVR_ATmega64__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega16A__) || defined(__AVR_ATmega323__) || defined (__AVR_ATmega163__) || defined (__AVR_ATmega8535__) || defined (__AVR_ATmega8__) || defined (__AVR_AT90CAN128__)
				REFERENCE_AREF = 0,
				REFERENCE_INTERNAL_AVCC = 0x40,
				REFERENCE_INTERNAL_2V56 = 0xc0
#else
				REFERENCE_AREF = 0,
				REFERENCE_INTERNAL_AVCC = 0x40,
				REFERENCE_INTERNAL_1V1 = 0xc0
#endif
// ATmega103, ATmega16HVB, ATmega32HVB, ATmega8HVA, ATmega16HVA, ATmega16HVA2, ATmega406 do no have selectable voltage reference
			};
			
			enum Prescaler
			{
				PRESCALER_2 = 0x01,
				PRESCALER_4 = 0x02,
				PRESCALER_8 = 0x03,
				PRESCALER_16 = 0x04,
				PRESCALER_32 = 0x05,
				PRESCALER_64 = 0x06,
				PRESCALER_128 = 0x07
			};
			
		public:
//------------ ADMUX register -------------------------------------------------
#if not defined VADC || defined (__DOXYGEN__)
			/**
			 * \brief voltage reference for the ADC
			 *
			 * The internal voltage reference options may not be used if an 
			 * external reference voltage is being applied to the AREF pin.
			 */
			static inline void
			setReferenceVoltage(Reference referenceVoltage)
			{
#if defined ISRCEN
				ADCSRB = (ADCSRB & ~(1<<AREFEN)) | (referenceVoltage & (1<<AREFEN));
#endif
				ADMUX = (ADMUX & ~0xc0) | (referenceVoltage & 0xc0);
			}
			
			/**
			 * \brief Change the presentation of the ADC conversion result
			 *
			 * Set to \c true to left adjust the result. Otherwise, the result
			 * is right adjusted.
			 * Change will affect the ADC Data Register immediately, regardless
			 * of any ongoing conversions.
			 */
			static inline void
			setLeftAdjustResult(bool enable)
			{
				ADMUX = (ADMUX & ~(1<<ADLAR)) | (enable ? (1<<ADLAR) : 0);
			}
#endif
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
			setChannel(uint8_t channel)
			{
#if defined ISRCEN
				if (channel > 0x12) return false;
#elif defined MUX5
				if (channel > 0x3f) return false;
				ADCSRB = (ADCSRB & ~(1<<MUX5)) | ((channel & 0x20) ? (1<<MUX5) : 0);
#elif defined (__AVR_ATmega103__)
				if (channel > 0x07) return false;
#elif not defined MUX4 || defined VADC
				if (channel > 0x0f) return false;
#else
				if (channel > 0x1f) return false;
#endif
				ADMUX = (ADMUX & ~0x1f) | (channel & 0x1f);
				return true;
			}
			
			/**
			 * \return The analog channel connected to the ADC
			 * 
			 * Available on all ATmegas.
			 */
			static inline uint8_t
			getChannel()
			{
#if defined MUX5
				return (ADMUX & 0x1f) | ((ADCSRB & (1<<MUX5)) ? 0x20 : 0);
#else
				return (ADMUX & 0x1f);
#endif
			}
			
//------------ ADCSRA register ------------------------------------------------
#if defined VADC
			static inline void
			setEnableAdc(bool enable)
			{
				VADCSR = (VADCSR & ~(1<<VADEN)) | (enable ? (1<<VADEN) : 0);
			}
			static inline void
			setStartConversion()
			{
				VADCSR |= (1<<VADSC);
			}
			static inline bool
			isInterruptFlagSet()
			{
				return (VADCSR & (1<<VADCCIF));
			}
			static inline void
			clearInterruptFlag()
			{
				VADCSR &= ~(1<<VADCCIF);
			}
			static inline void
			setInterruptEnable(bool enable)
			{
				VADCSR = (VADCSR & ~(1<<VADCCIE)) | (enable ? (1<<VADCCIE) : 0);
			}
#else
			/**
			 * \brief Enables the ADC
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			setEnableAdc(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADEN)) | (enable ? (1<<ADEN) : 0);
			}
			/**
			 * In Single Conversion mode, this starts each conversion.
			 * In Free Running mode, this starts the first conversion.
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			setStartConversion()
			{
				ADCSRA |= (1<<ADSC);
			}
#if defined ADATE || defined (__DOXYGEN__)
			/**
			 * \brief Enable auto triggering of the ADC
			 *
			 * The ADC will start a conversion on a positive edge of the
			 * selected trigger signal.
			 * \see setAutoTriggerSource
			 */
			static inline void
			setAutoTriggerEnable(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADATE)) | (enable ? (1<<ADATE) : 0);
			}
#endif
#if defined ADFR || defined (__DOXYGEN__)
			/**
			 * \brief Enables free running mode
			 *
			 * The ADC will continously start conversions and provide the most
			 * recent result in the ADC register.
			 */
			static inline void
			setFreeRunningMode(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADFR)) | (enable ? (1<<ADFR) : 0);
			}
#endif
			/**
			 * \return \c true if the flag is set
			 *
			 * Available on all ATmegas.
			 */
			static inline bool
			isInterruptFlagSet()
			{
				return (ADCSRA & (1<<ADIF));
			}
			/**
			 * \brief Clears the interrupt flag
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			clearInterruptFlag()
			{
				ADCSRA &= ~(1<<ADIF);
			}
			/**
			 * \brief Enables the ADC Conversion Complete Interrupt
			 *
			 * Available on all ATmegas.
			 *
			 * \see AdcInterrupt
			 */
			static inline void
			setInterruptEnable(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADIE)) | (enable ? (1<<ADIE) : 0);
			}
			/**
			 * Set the division factor between the system clock frequency
			 * and the input clock to the ADC.
			 */
			static inline void
			setPrescaler(Prescaler prescaler)
			{
				ADCSRA = (ADCSRA & ~0x07) | prescaler;
			}
#endif
			
			
//------------ ADCSRB register ------------------------------------------------
#if defined ADCSRB || defined (__DOXYGEN__)
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
			setAutoTriggerSource(uint8_t source)
			{
#if defined ISRCEN
				if (source > 0x0d) return;
#else
				if (source > 0x07) return;
#endif
				ADCSRB = (ADCSRB & ~0x0f) | source;
			}
#elif defined SFIOR
			static inline void
			setAutoTriggerSource(uint8_t source)
			{
				SFIOR = (SFIOR & ~0xe0) | (source & 0xe0);
			}
#endif
			
#if defined ISRCEN || defined (__DOXYGEN__)
			/**
			 * \brief Enable the ADC High Speed mode
			 *
			 * Set this if you wish to convert with an ADC clock frequency
			 * higher than 200KHz.
			 * Clear this bit to reduce the power consumption of the ADC when
			 * the ADC clock frequency is lower than 200KHz.
			 *
			 * Only Available on the M- and C-series ATmega's!
			 */
			static inline void
			setHighSpeedMode(bool enable)
			{
				ADCSRB = (ADCSRB & ~(1<<ADHSM)) | (enable ? (1<<ADHSM) : 0);
			}
			/**
			 * \brief Source a 100μA current to the AREF pin
			 *
			 * Clear this bit to use AREF pin as Analog Reference pin.
			 *
			 * Only Available on the M- and C-series ATmega's!
			 */
			static inline void
			setCurrentSourceEnable(bool enable)
			{
				ADCSRB = (ADCSRB & ~(1<<ISRCEN)) | (enable ? (1<<ISRCEN) : 0);
			}
#endif
			
#if defined (__AVR_ATmega128RFA1__) || defined (__DOXYGEN__)
			/**
			 * The analog functions of the ADC are powered from the AVDD domain.
			 * AVDD is supplied from an internal voltage regulator.
			 * Enabling the ADC will power-up the AVDD domain if not already
			 * requested by another functional group of the device.
			 * This method allows the user to monitor (poll) the status of the
			 * AVDD domain.
			 *
			 * \return \c true indicates that AVDD has been powered-up.
			 *
			 * Only Available on the ATmega128RFA1!
			 */
			static inline bool
			isAvddOk()
			{
				return (ADCSRB & (1<<AVDDOK));
			}
			/**
			 * The status of the internal generated reference voltage can be 
			 * monitored through this bit.
			 * After enabling the ADC and setting the reference voltage, it
			 * will be available after a start-up delay.
			 * 
			 * \return \c true which indicates that the internal generated
			 * reference voltage is approaching final levels.
			 *
			 * Only Available on the ATmega128RFA1!
			 */
			static inline bool
			isReferenceVoltageOk()
			{
				return (ADCSRB & (1<<REFOK));
			}
			/**
			 * The user can force a reset of the analog blocks by setting this
			 * without requesting a different channel. The analog blocks of the
			 * ADC will be reset to handle possible new voltage ranges. Such a
			 * reset phase is especially important for the gain amplifier. It
			 * could be temporarily disabled by a large step of its input common
			 * voltage leading to erroneous A/D conversion results.
			 *
			 * Only Available on the ATmega128RFA1!
			 */
			static inline void
			setAnalogChannelChange()
			{
				ADCSRB |= (1<<ACCH);
			}
#endif
			
//------------ ADC register ---------------------------------------------------
			static inline uint16_t
			getDataRegister()
			{
#if defined VADC
				return VADC;
#else
				return ADC;
#endif
			}
			
//-----------------------------------------------------------------------------
#if defined VADC
			static inline void
			initialize()
			{
				setEnableAdc(true);
			}
#else
			/**
			 * \brief	Initialize and enable the A/D converter
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			initialize(Reference referenceVoltage,
					   Prescaler prescaler)
			{
				setReferenceVoltage(referenceVoltage);
				setPrescaler(prescaler);
				setEnableAdc(true);
			}
#endif
			
			/**
			 * \brief Correctly enables Free Running Mode
			 *
			 * Available on all ATmegas.
			 */
			static inline void
			enableFreeRunningMode()
			{
#if defined ADFR
				setFreeRunningMode(true);
#else
				setAutoTriggerEnable(true);
				setAutoTriggerSource(0);
#endif
			}
			
			/**
			 * \brief	Read the value an analog channel
			 * 
			 * A normal conversion takes 13 ADC clock cycles. With a clock frequency
			 * of for example 200 kHz a conversion therefore needs 65 microseconds.
			 * This time increases with a lower frequency.
			 *
			 * Available on all ATmegas.
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
			 *
			 * Available on all ATmegas.
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

#endif	// XPCC_ATMEGA__ADC_HPP
