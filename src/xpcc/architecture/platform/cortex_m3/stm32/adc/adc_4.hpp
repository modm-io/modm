// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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
/** \file adc_4.hpp
 *
 *	The documentation is compiled for STM32F4XX.
 *
 *  @date	03.04.2012
 *  @author	David Hebbeker
 */

// macro for using enum classes as flags
// TODO: move to global file
#define ENUM_CLASS_FLAG(name) \
inline name operator|(name a, name b) \
{return static_cast<name>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));} \
inline uint32_t operator&(name a, name b) \
{return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b));} \
inline uint32_t operator&(uint32_t a, name b) \
{return ((a) & static_cast<uint32_t>(b));} \
inline uint32_t operator&(name a, uint32_t b) \
{return (static_cast<uint32_t>(a) & (b));}


#ifndef XPCC_STM32__ADC4_HPP
#define XPCC_STM32__ADC4_HPP

#if defined(__DOXYGEN__)
	#if !defined(STM32F4XX)
		/** Symbol defined to select the platform for which the documentation is 
		 *	build.
		 */
		#define STM32F4XX	1
	#endif
#endif


#if defined(STM32F4XX) || defined(STM32F3XX) || defined(STM32F2XX) || \
	defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(__DOXYGEN__)
#include <xpcc/driver/interface.hpp>
#include <stdint.h>
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
#if defined(STM32F3XX)
/* id < 4 */

		/**
		 * Analog/Digital-Converter module (ADC4).
		 *
		 * The 12-bit ADC is a successive approximation analog-to-digital
		 * converter. It has up to 19 multiplexed channels allowing it measure
		 * signals from 16 external and three internal sources.
		 * The result of the ADC is stored in a left-aligned or right-aligned
		 * 16-bit data register.
		 *
		 * This API is designed for the internal ADCs of STM32F30X/STM32F31X
		 *
		 * \author	Kevin Laeufer
		 * \ingroup	stm32
		 */
		class Adc4 : public Interface
		{
		public:
			/**
			 * Channels, which can be used with this ADC.
			 *
			 * You can specify the channel by using a pin-name, like PIN_C0, an
			 * internal sensor, like TEMPERATURE_SENSOR or just the plain
			 * channel number, like CHANNEL_0.
			 */
			enum class Channel : uint8_t	// TODO: What is the best type?
			{
				Channel1  =  1,
				Channel2  =  2,
				Channel3  =  3,
				Channel4  =  4,
				Channel5  =  5,
				Channel6  =  6,
				Channel7  =  7,
				Channel8  =  8,
				Channel9  =  9,
				Channel10 = 10,
				// channels 11-16 are not available on all ADCs
				Channel17 = 17,
				Channel18 = 18,
				PinE14 =  1,
				PinE15 =  2,
				PinB12 =  3,
				PinB14 =  4,
				PinB15 =  5,
				PinE8  =  6,
				PinD10 =  7,
				PinD11 =  8,
				PinD12 =  9,
				PinD13 = 10,
				Channel11 = 11,
				PinD14 = 11,
				Channel12 = 12,
				PinD8  = 12,
				Channel13 = 13,
				PinD9  = 13,
				// 14-16 reserved
				Opamp3 = 17,
				InternalReference = 18,
			};

			enum class ClockMode : uint32_t
			{
				Asynchronous = 0,	// clocked by ADC_CK12 / ADC_CK34
				// In synchronous mode ADC is clocked by the AHB clock
				SynchronousPrescaler1 = ADC12_CCR_CKMODE_0,
				SynchronousPrescaler2 = ADC12_CCR_CKMODE_1,
				SynchronousPrescaler4 = ADC12_CCR_CKMODE_1 | ADC12_CCR_CKMODE_0,
				DoNotChange = 0xff,// if you do not want to change the clock mode
			};

			// Prescaler of the Asynchronous ADC clock
			enum class Prescaler : uint32_t
			{
				Disabled 			= 0,
				
				Div1 				= RCC_CFGR2_ADCPRE34_DIV1,
				Div2 				= RCC_CFGR2_ADCPRE34_DIV2,
				Div4 				= RCC_CFGR2_ADCPRE34_DIV4,
				Div6 				= RCC_CFGR2_ADCPRE34_DIV6,
				Div8 				= RCC_CFGR2_ADCPRE34_DIV8,
				Div10 				= RCC_CFGR2_ADCPRE34_DIV10,
				Div12 				= RCC_CFGR2_ADCPRE34_DIV12,
				Div16 				= RCC_CFGR2_ADCPRE34_DIV16,
				Div32 				= RCC_CFGR2_ADCPRE34_DIV32,
				Div64 				= RCC_CFGR2_ADCPRE34_DIV64,
				Div128 				= RCC_CFGR2_ADCPRE34_DIV128,
				Div256 				= RCC_CFGR2_ADCPRE34_DIV256,
				Div256AllBits 		= RCC_CFGR2_ADCPRE34,	// for bit clear
			};

			enum class SampleTime : uint8_t	// TODO: What is the best type?
			{
				Cycles2 	= 0b000,	//!   1.5 ADC clock cycles
				Cycles3 	= 0b001,	//!   2.5 ADC clock cycles
				Cycles5 	= 0b010,	//!   4.5 ADC clock cycles
				Cycles8 	= 0b011,	//!   7.5 ADC clock cycles
				Cycles20 	= 0b100,	//!  19.5 ADC clock cycles
				Cycles62 	= 0b101,	//!  61.5 ADC clock cycles
				Cycles182 	= 0b110,	//! 181.5 ADC clock cycles
				Cycles602 	= 0b111,	//! 601.5 ADC clock cycles
			};

			enum class CalibrationMode : uint32_t
			{
				SingleEndedInputsMode = 0,
				DifferntialInputsMode = ADC_CR_ADCALDIF,
				DoNotCalibrate = 0xff,	// if you want to avoid calibration
			};

			enum class VoltageRegulatorState : uint32_t
			{
				// Intermediate state is needed to move from enabled to disabled
				// state and vice versa
				Intermediate 	= 0,
				Enabled 		= ADC_CR_ADVREGEN_0,
				Disables		= ADC_CR_ADVREGEN_1,
			};

			enum class Interrupt : uint32_t
			{
				Ready 								= ADC_IER_RDY,
				EndOfSampling 						= ADC_IER_EOSMP,
				EndOfRegularConversion 				= ADC_IER_EOC,
				EndOfRegularSequenceOfConversions 	= ADC_IER_EOS,
				Overrun 							= ADC_IER_OVR,
				EndOfInjectedConversion 			= ADC_IER_JEOC,
				EndOfInjectedSequenceOfConversions 	= ADC_IER_JEOS,
				AnalogWatchdog1 					= ADC_IER_AWD1,
				AnalogWatchdog2 					= ADC_IER_AWD2,
				AnalogWatchdog3 					= ADC_IER_AWD3,
				InjectedContextQueueOverflow 		= ADC_IER_JQOVF,
			};

			enum class InterruptFlag : uint32_t
			{
				Ready 								= ADC_ISR_ADRD,
				EndOfSampling 						= ADC_ISR_EOSMP,
				EndOfRegularConversion 				= ADC_ISR_EOC,
				EndOfRegularSequenceOfConversions 	= ADC_ISR_EOS,
				Overrun 							= ADC_ISR_OVR,
				EndOfInjectedConversion 			= ADC_ISR_JEOC,
				EndOfInjectedSequenceOfConversions 	= ADC_ISR_JEOS,
				AnalogWatchdog1 					= ADC_ISR_AWD1,
				AnalogWatchdog2 					= ADC_ISR_AWD2,
				AnalogWatchdog3 					= ADC_ISR_AWD3,
				InjectedContextQueueOverflow 		= ADC_ISR_JQOVF,
			};

		public:
			/**
			 * Initialize and enable the A/D converter.
			 *
			 * Enables the ADC clock and switches on the ADC. The ADC clock
			 * prescaler will be set as well.
			 *
			 * The ADC can be clocked
			 *
			 * @param clk
			 * 		Clock Mode for ADC1/ADC2 or ADC3/ADC4.
			 * 		Set to ClockMode::DoNotChange or leave blank if you
			 * 		want to leave this setting untouched.
			 *
			 * @param pre
			 * 		The prescaler for the asynchronous ADC clock.
			 * 		This parameter is only taken into account
			 * 		if clk == ClockMode::Asynchronous.
			 */
			static void
			initialize(const ClockMode clk = ClockMode::DoNotChange,
					const Prescaler pre = Prescaler::Disabled,
					const CalibrationMode cal = CalibrationMode::DoNotCalibrate,
					const bool blocking = true);

			static void inline
			disable(const bool blocking = true)
			{
				ADC4->CR |= ADC_CR_ADDIS;
				if (blocking) {
					// wait for ADC_CR_ADDIS to be cleared by hw
					while(ADC4->CR & ADC_CR_ADDIS);
				}
				// disable clock
				RCC->AHBENR &= ~RCC_AHBENR_ADC34EN;
			}

			/**
			 * Select the frequency of the clock to the ADC. The clock is common
			 * for ADC1/ADC2 and ADC3/ADC4.
			 *
			 * @pre The PLL must be running.
			 *
			 * @param prescaler
			 * 		The prescaler specifies by which factor the system clock
			 * 		will be divided.
			 */
			static inline void
			setPrescaler(const Prescaler pre)
			{
				uint32_t tmp;
				tmp  = RCC->CFGR2;
				tmp &= ~static_cast<uint32_t>(Prescaler::Div256AllBits);
				tmp |=  static_cast<uint32_t>(pre);
				RCC->CFGR2 = tmp;
			}

			/**
			 * Returns true if the ADRDY bit of the ISR is set
			 **/
			static inline bool
			isReady()
			{
				return (static_cast<uint32_t>(getInterruptFlags()) &
								static_cast<uint32_t>(InterruptFlag::Ready));
			}

			static inline void
			calibrate(const CalibrationMode mode,
											const bool blocking = true)
			{
				if (mode != CalibrationMode::DoNotCalibrate) {
					ADC4->CR |= ADC_CR_ADCAL |
													static_cast<uint32_t>(mode);
					if(blocking) {
						// wait for ADC_CR_ADCAL to be cleared by hw
						while(ADC4->CR & ADC_CR_ADCAL);
					}
				}
			}

			/**
			 * Change the presentation of the ADC conversion result.
			 *
			 * @param enable
			 * 		Set to \c true to left adjust the result.
			 *		Otherwise, the result is right adjusted.
			 *
			 * @pre The ADC clock must be started and the ADC switched on with
			 * 		initialize()
			 */
			static inline void
			setLeftAdjustResult(const bool enable)
			{
				if (enable) {
					ADC4->CFGR |= ADC_CFGR_ALIGN;
				}
				else {
					ADC4->CFGR &= ~ADC_CFGR_ALIGN;
				}
			}

			/**
			 * Analog channel selection.
			 *
			 * This not for scan mode. The number of channels will be set to 1,
			 * the channel selected and the corresponding pin will be set to
			 * analog input.
			 * If the the channel is modified during a conversion, the current
			 * conversion is reset and a new start pulse is sent to the ADC to
			 * convert the new chosen channnel / group of channels.
			 *
			 *
			 * @param channel		The channel which shall be read.
			 * @param sampleTime	The sample time to sample the input voltage.
			 *
			 * @pre The ADC clock must be started and the ADC switched on with
			 * 		initialize()
			 */
			static void
			setChannel(const Channel channel,
					const SampleTime sampleTime=static_cast<SampleTime>(0b000))
			{
				uint32_t tmpreg;
				// SQR1[10:6] contain SQ1[4:0]
				ADC4->SQR1 = (static_cast<uint8_t>(channel) & 0b11111) << 6;

				if (static_cast<uint8_t>(channel) < 10) {
					tmpreg = ADC4->SMPR1
						& ((~ADC_SMPR1_SMP0) << (static_cast<uint8_t>(channel) * 3));
					tmpreg |= static_cast<uint32_t>(sampleTime) <<
									(static_cast<uint8_t>(channel) * 3);
					ADC4->SMPR1 = tmpreg;
				}
				else {
					tmpreg = ADC4->SMPR2
						& ((~ADC_SMPR2_SMP10) << ((static_cast<uint8_t>(channel)-10) * 3));
					tmpreg |= static_cast<uint32_t>(sampleTime) <<
									((static_cast<uint8_t>(channel)-10) * 3);
					ADC4->SMPR2 = tmpreg;
				}
			}

			/**
			 * Enables free running mode
			 *
			 * The ADC will continously start conversions and provide the most
			 * recent result in the ADC register.
			 *
			 * @pre The ADC clock must be started and the ADC switched on with
			 * 		initialize()
			 */
			static inline void
			setFreeRunningMode(const bool enable)
			{
				if (enable) {
					ADC4->CFGR |=  ADC_CFGR_CONT; // set to continuous mode
				} else {
					ADC4->CFGR &= ~ADC_CFGR_CONT; // set to single mode
				}
			}

			/**
			 * Start a new conversion or continuous conversions.
			 *
			 * @pre A ADC channel must be selected with setChannel().
			 *
			 * @post The result can be fetched with getValue()
			 *
			 * TODO: is there any limitation to when is can be called??
			 */
			static inline void
			startConversion(void)
			{
				// TODO: maybe add more interrupt flags
				resetInterruptFlags(static_cast<InterruptFlag>(
					static_cast<uint32_t>(InterruptFlag::EndOfSampling) |
					static_cast<uint32_t>(InterruptFlag::Overrun)));
				// starts single conversion for the regular group
				ADC4->CR |= ADC_CR_ADSTART;
			}

			/**
			 * @return If the conversion is finished.
			 * @pre A conversion should have been stared with startConversion()
			 */
			static inline bool
			isConversionFinished(void)
			{
				return (static_cast<uint32_t>(getInterruptFlags()) &
						static_cast<uint32_t>(InterruptFlag::EndOfSampling));
			}

			/**
			 * @return The most recent 16bit result of the ADC conversion.
			 * @pre A conversion should have been stared with startConversion()
			 *
			 * To have a blocking GET you might do it this way:
			 * @code
				while(!isConversionFinished())
				{
					// Waiting for conversion
				}
				@endcode
			 */
			static inline uint16_t
			getValue(void)
			{
				return ADC4->DR;
			}

		private:
			// ----------------------------------------------------------------------------
			// TODO: move this to some shared header for all cortex m3 platforms
			// Re-implemented here to save some code space. As all arguments in the calls
			// below are constant the compiler is able to calculate everything at
			// compile time.
			static ALWAYS_INLINE void
			nvicEnableInterrupt(const IRQn_Type IRQn)
			{
				NVIC->ISER[(static_cast<uint32_t>(IRQn) >> 5)] =
											(1 << ((uint32_t)(IRQn) & 0x1F));
			}

			static ALWAYS_INLINE void
			nvicDisableInterrupt(IRQn_Type IRQn)
			{
				NVIC_DisableIRQ(IRQn);
			}

		public:
			static void
			enableInterruptVector(const uint32_t priority,
													const bool enable = true)
			{
				const IRQn_Type INTERRUPT_VECTOR = ADC4_IRQn;
				if (enable) {
					NVIC_SetPriority(INTERRUPT_VECTOR, priority);
					nvicEnableInterrupt(INTERRUPT_VECTOR);
				} else {
					NVIC_DisableIRQ(INTERRUPT_VECTOR);
				}
			}

			static inline void
			enableInterrupt(const Interrupt interrupt)
			{
				ADC4->IER |= static_cast<uint32_t>(interrupt);
			}

			static inline void
			disableInterrupt(const Interrupt interrupt)
			{
				ADC4->IER &= ~static_cast<uint32_t>(interrupt);
			}

			static inline InterruptFlag
			getInterruptFlags()
			{
				return static_cast<InterruptFlag>(ADC4->ISR);
			}

			static inline void
			resetInterruptFlags(const InterruptFlag flags)
			{
				// Flags are cleared by writing a one to the flag position.
				// Writing a zero is ignored.
				ADC4->ISR = static_cast<uint32_t>(flags);
			}
		};

		ENUM_CLASS_FLAG(Adc4::InterruptFlag)
#else
	#  error "Unknown CPU Type. Please define STM32F10X_.., \
											STM32F2XX, STM32F3XX or STM32F4XX"
#endif
	}

}


#endif // defined(STM32F2XX) || defined(STM32F4XX) || defined (STM32F10X_HD) || defined (STM32F10X_XL)
#endif	// XPCC_STM32__ADC4_HPP