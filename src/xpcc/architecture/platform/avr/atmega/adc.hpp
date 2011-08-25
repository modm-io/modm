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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATMEGA__ADC_HPP
#define XPCC_ATMEGA__ADC_HPP

#include <avr/io.h>

namespace xpcc
{
	namespace atmega
	{
		/**
		 * \brief	Analog/Digital-converter
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
		 *     xpcc::Adc::initialize(
		 *             xpcc::Adc::REFERENCE_INTERNAL_2V56,
		 *             xpcc::Adc::PRESCALER_64);
		 *     
		 *     ...
		 *     // read channel 2
		 *     uint16_t value = xpcc::Adc::readChannel(2);
		 *     ...
		 * }
		 * \endcode
		 * 
		 * Asynchronous mode:
		 * \code
		 * // start a new conversion on channel 3
		 * xpcc::Adc::startConversion(3);
		 * 
		 * // do something usefull while waiting for the result 
		 * while (!xpcc::Adc::isFinished())
		 *     ;
		 * 
		 * // read the converted value
		 * uint16_t value = xpcc::Adc::getValue();
		 * \endcode
		 * 
		 * For a detailed example see the \c adc folder in the examples folder.
		 * 
		 * \author	Fabian Greif, Niklas Hauser
		 * \ingroup	atmega
		 */
		class Adc
		{
		public:
#if defined (__AVR_ATmega64HVE__)
	#error "The ATmega64HVE is not supported by this ADC class."
#endif
#if defined ADCSR
	#define ADCSRA ADCSR
#endif
#if defined VADMUX
	#define ADMUX VADMUX
#endif
			enum Reference
			{
#if defined (__AVR_ATmega16M1__) || defined (__AVR_ATmega32M1__) || defined (__AVR_ATmega64M1__) || defined (__AVR_ATmega32C1__) || defined (__AVR_ATmega64C1__)
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
#elif defined (__AVR_ATmega128__) || defined (__AVR_ATmega64__) || defined (__AVR_ATmega32__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega16A__) || (__AVR_ATmega323__) || defined (__AVR_ATmega163__) || defined (__AVR_ATmega8535__) || defined (__AVR_ATmega8__)
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
#if not defined (__AVR_ATmega103__) || not defined (__AVR_ATmega16HVB__) || not defined (__AVR_ATmega32HVB__) || not defined (__AVR_ATmega8HVA__)  || not defined (__AVR_ATmega16HVA__)  || not defined (__AVR_ATmega16HVA2__) || not defined (__AVR_ATmega406__)
			static inline void
			setReferenceVoltage(Reference referenceVoltage)
			{
#if defined (__AVR_ATmega16M1__) || defined (__AVR_ATmega32M1__) || defined (__AVR_ATmega64M1__) || defined (__AVR_ATmega32C1__) || defined (__AVR_ATmega64C1__)
				ADCSRB = (ADCSRB & ~(1<<AREFEN)) | (referenceVoltage & (1<<AREFEN));
#endif
				ADMUX = (ADMUX & ~0xc0) | (referenceVoltage & 0xc0);
			}
			
			static inline void
			setLeftAdjustResult(bool enable)
			{
				ADMUX = (ADMUX & ~(1<<ADLAR)) | (enable ? (1<<ADLAR) : 0);
			}
#endif

			static inline bool
			setChannel(uint8_t channel)
			{
#if defined (__AVR_ATmega16M1__) || defined (__AVR_ATmega32M1__) || defined (__AVR_ATmega64M1__) || defined (__AVR_ATmega32C1__) || defined (__AVR_ATmega64C1__)
				if (channel > 0x12) return false;
#elif defined (MUX5)
				if (channel > 0x3f) return false;
				ADCSRB = (ADCSRB & ~(1<<MUX5)) | ((channel & 0x20) ? (1<<MUX5) : 0);
#elif defined (__AVR_ATmega103__)
				if (channel > 0x07) return false;
#elif not defined (MUX4)
				if (channel > 0x0f) return false;
#else
				if (channel > 0x1f) return false;
#endif
				ADMUX = (ADMUX & ~0x1f) | (channel & 0x1f);
				return true;
			}
			
			static inline uint8_t
			getChannel()
			{
#if defined (MUX5)
				return (ADMUX & 0x1f) | ((ADCSRB & (1<<MUX5)) ? 0x20 : 0);
#else
				return (ADMUX & 0x1f);
#endif
			}
			
//------------ ADCSRA register ------------------------------------------------
#if defined VADCSR
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
			static inline void
			setEnableAdc(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADEN)) | (enable ? (1<<ADEN) : 0);
			}
			static inline void
			setStartConversion()
			{
				ADCSRA |= (1<<ADSC);
			}
#ifdef ADATE
			static inline void
			setAutoTriggerEnable(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADATE)) | (enable ? (1<<ADATE) : 0);
			}
#elif defined ADFR
			static inline void
			setFreeRunningMode(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADFR)) | (enable ? (1<<ADFR) : 0);
			}
#endif
			static inline bool
			isInterruptFlagSet()
			{
				return (ADCSRA & (1<<ADIF));
			}
			static inline void
			clearInterruptFlag()
			{
				ADCSRA &= ~(1<<ADIF);
			}
			static inline void
			setInterruptEnable(bool enable)
			{
				ADCSRA = (ADCSRA & ~(1<<ADIE)) | (enable ? (1<<ADIE) : 0);
			}
			static inline void
			setPrescaler(Prescaler prescaler)
			{
				ADCSRA = (ADCSRA & ~0x07) | prescaler;
			}
#endif
			
			
//------------ ADCSRB register ------------------------------------------------
#if defined ADCSRB
			static inline void
			setAutoTriggerSource(uint8_t source)
			{
#if defined (__AVR_ATmega16M1__) || defined (__AVR_ATmega32M1__) || defined (__AVR_ATmega64M1__) || defined (__AVR_ATmega32C1__) || defined (__AVR_ATmega64C1__)
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
			
#if defined (__AVR_ATmega16M1__) || defined (__AVR_ATmega32M1__) || defined (__AVR_ATmega64M1__) || defined (__AVR_ATmega32C1__) || defined (__AVR_ATmega64C1__)
			static inline void
			setHighSpeedMode(bool enable)
			{
				ADCSRB = (ADCSRB & ~(1<<ADHSM)) | (enable ? (1<<ADHSM) : 0);
			}
			static inline void
			setCurrentSourceEnable(bool enable)
			{
				ADCSRB = (ADCSRB & ~(1<<ISRCEN)) | (enable ? (1<<ISRCEN) : 0);
			}
#endif
			
#if defined (__AVR_ATmega128RFA1__)
			static inline void
			isAvddOk()
			{
				return (ADCSRB & (1<<AVDDOK));
			}
			static inline void
			isReferenceVoltageOk()
			{
				return (ADCSRB & (1<<REFOK));
			}
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
			/**
			 * \brief	Initialize the A/D converter
			 */
#if not defined (__AVR_ATmega103__) || not defined (__AVR_ATmega16HVB__) || not defined (__AVR_ATmega32HVB__) || not defined (__AVR_ATmega8HVA__)  || not defined (__AVR_ATmega16HVA__)  || not defined (__AVR_ATmega16HVA2__) || not defined (__AVR_ATmega406__)
			static inline void
			initialize(Reference referenceVoltage,
					   Prescaler prescaler)
			{
				setReferenceVoltage(referenceVoltage);
				setPrescaler(prescaler);
			}
#else
			static inline void
			initialize(Prescaler prescaler)
			{
				setPrescaler(prescaler);
			}
#endif
			
			/**
			 * \brief	Read the value an analog channel
			 * 
			 * A normal conversion takes 13 ADC clock cycles. With a clock frequency
			 * of for example 200 kHz a conversion therefore needs 65 microseconds.
			 * This time increases with a lower frequency.
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
			
			/// Start a new conversion
			static inline bool
			startConversion(uint8_t channel)
			{
				if (!setChannel(channel)) return false;
				
				clearInterruptFlag();
				setStartConversion();
				
				return true;
			}
			
			/// Check if the conversion is finished
			static inline bool
			isFinished()
			{
				return isInterruptFlagSet();
			}
			
			/// Read the converted analog value
			static inline uint16_t
			getValue()
			{
				return getDataRegister();
			}
		};
	}
}

#endif	// XPCC_ATMEGA__ADC_HPP
