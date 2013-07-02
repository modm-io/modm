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
/** \file adc_1.hpp
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


#ifndef XPCC_STM32__ADC1_HPP
#define XPCC_STM32__ADC1_HPP

#if defined(__DOXYGEN__)
	#if !defined(STM32F4XX)
		/** Symbol defined to select the platform for which the documentation is 
		 *	build.
		 */
		#define STM32F4XX	1
	#endif
#endif

#include <xpcc/driver/interface.hpp>
#include <stdint.h>
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{

#if defined(STM32F4XX) || defined(STM32F2XX) || defined(STM32F10X)
		/**
		 * Analog/Digital-Converter module (ADC1).
		 * 
		 * The 12-bit ADC is a successive approximation analog-to-digital
		 * converter. It has up to 18 multiplexed channels allowing it measure
		 * signals from 16 external and two internal sources.
		 * The result of the ADC is stored in a left-aligned or right-aligned
		 * 16-bit data register.
		 * 
		 * This API is designed for the internal ADCs of STM32F4XX, 
		 * STM32F10X_LD, STM32F10X_LD_VL, STM32F10X_MD, STM32F10X_MD_VL, 
		 * STM32F10X_HD, STM32F10X_HD_VL, STM32F10X_XL and STM32F10X_CL.
		 * 
		 * \author	Stephan Kugelmann, David Hebbeker
		 * \ingroup	stm32
		 */
		class Adc1 : public Interface
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
				PIN_A0 = 0,
				PIN_A1 = 1,
				PIN_A2 = 2,
				PIN_A3 = 3,
				PIN_C0 = 10,
				PIN_C1 = 11,
				PIN_C2 = 12,
				PIN_C3 = 13,
				// For ADC1 and ADC2
				PIN_A4 = 4,
				PIN_A5 = 5,
				PIN_A6 = 6,
				PIN_A7 = 7,
				PIN_B0 = 8,
				PIN_B1 = 9,
				PIN_C4 = 14,
				PIN_C5 = 15,
				#if defined(STM32F2XX) || defined(STM32F4XX)				
				/** Flag to show, that the temperature and V_Ref measurements 
				 * 	are available for this ADC.
				 */ 
				#define TEMPERATURE_REFVOLTAGE_AVIALABLE (1) 
				
				/**
				 * The half V_BAT voltage.
				 */
				VBAT = 18,
				
				#elif defined(STM32F10X)				
				/** Flag to show, that the temperature and V_Ref measurements 
				 * 	are available for this ADC.
				 */ 
				#define TEMPERATURE_REFVOLTAGE_AVIALABLE (1)
				#endif
				
#ifdef TEMPERATURE_REFVOLTAGE_AVIALABLE
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
#endif // TEMPERATURE_REFVOLTAGE_AVIALABLE
				
				CHANNEL_0 = 0,
				CHANNEL_1 = 1,
				CHANNEL_2 = 2,
				CHANNEL_3 = 3,
				CHANNEL_4 = 4,
				CHANNEL_5 = 5,
				CHANNEL_6 = 6,
				CHANNEL_7 = 7,
				CHANNEL_8 = 8,
				CHANNEL_10= 10,
				CHANNEL_11 = 11,
				CHANNEL_12 = 12,
				CHANNEL_13 = 13,
#if defined(STM32F2XX) || defined(STM32F4XX)
				CHANNEL_9 = 9,
				CHANNEL_14 = 14,
				CHANNEL_15 = 15,
				CHANNEL_16 = 16,
				CHANNEL_17 = 17,
				CHANNEL_18 = 18
#elif defined(STM32F10X)
				// For ADC1 and ADC2
				CHANNEL_9 = 9,
				CHANNEL_14 = 14,
				CHANNEL_15 = 15,
				CHANNEL_16 = 16,	//!< connected to temperature sensor
				CHANNEL_17 = 17		//!< connected to V_REFINT
				#endif
			};
			
			/**
			 * Programmable prescaler to divide the APB2 clock frequency, which 
			 * is used fot the analog circuitry (not the digital interface which
			 * is used for registers). 
			 */
			enum Prescaler
			{
				PRESCALER_2 = 0b00,	//!< PCLK2 divided by 2
				PRESCALER_4 = 0b01,	//!< PCLK2 divided by 4
				PRESCALER_6 = 0b10,	//!< PCLK2 divided by 6
				PRESCALER_8 = 0b11	//!< PCLK2 divided by 8
			};
			
			/**
			 * Sampling time of the input voltage.
			 * 
			 * Total conversion time is T_con = Sampling time + 12 cycles
			 */
			enum SampleTime
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				CYCLES_3 	= 0b000,	//!< 3 ADCCLK cycles
				CYCLES_15 	= 0b001,	//!< 15 ADCCLK cycles
				CYCLES_28 	= 0b010,	//!< 28 ADCCLK cycles
				CYCLES_56 	= 0b011,	//!< 56 ADCCLK cycles
				CYCLES_84 	= 0b100,	//!< 84 ADCCLK cycles
				CYCLES_112 	= 0b101,	//!< 112 ADCCLK cycles
				CYCLES_144 	= 0b110,	//!< 144 ADCCLK cycles
				CYCLES_480 	= 0b111		//!< 480 ADCCLK cycles
#elif defined(STM32F10X)
				CYCLES_2 	= 0b000,	//!< 1.5 ADCCLK cycles
				CYCLES_8 	= 0b001,	//!< 7.5 ADCCLK cycles
				CYCLES_14 	= 0b010,	//!< 13.5 ADCCLK cycles
				CYCLES_29 	= 0b011,	//!< 28.5 ADCCLK cycles
				CYCLES_42 	= 0b100,	//!< 41.5 ADCCLK cycles
				CYCLES_56 	= 0b101,	//!< 55.5 ADCCLK cycles
				CYCLES_72 	= 0b110,	//!< 71.5 ADCCLK cycles
				CYCLES_240 	= 0b111		//!< 239.5 ADCCLK cycles
#endif
			}; 

			/**
			 * Possible interrupts.
			 * 
			 * An interrupt can be produced on the end of conversion for regular
			 * and injected groups, when the analog watchdog status bit is set 
			 * and when the overrun status bit is set. 
			 */
			enum Interrupt
			{
				END_OF_CONVERSION_REGULAR	= ADC_SR_EOC,	//!< End of conversion of a regular group
				END_OF_CONVERSION_INJECTED	= ADC_SR_JEOC,	//!< End of conversion of an injected group
				ANALOG_WATCHDOG				= ADC_SR_AWD,	//!< Analog watchdog status bit is set
#if defined(STM32F2XX) || defined(STM32F4XX) 
				OVERRUN						= ADC_SR_OVR	//!< Overrun (if data are lost)
#endif
			};

		public:
			/**
			 * Initialize and enable the A/D converter.
			 *
			 * Enables the ADC clock and switches on the ADC. The ADC clock
			 * prescaler will be set as well.
			 * 
			 * The ADC clock must not exceed 14 MHz. Default is a prescaler
			 * of 8 which gives a 9 Mhz ADC clock at 72 MHz APB2 clock.
			 * 
			 * @param prescaler
			 * 		The prescaler specifies by which factor the 
			 * 		APB2 clock (72 MHz) will be divided.
			 */
			static inline void
			initialize(Prescaler prescaler=PRESCALER_8)
			{
				// Initialize ADC
				RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;	// start ADC Clock
				ADC1->CR2 |= ADC_CR2_ADON;			// switch on ADC
				setPrescaler(prescaler);
			}
			
			// TODO
			//static void
			//calibrate();
			
			/** 
			 * Select the frequency of the clock to the ADC. The clock is common
			 * for all the ADCs (ADC1, ADC2, ADC3) and all channels. 
			 * 
			 * @pre The ADC clock must be started and the ADC switched on with 
			 * 	initialize()
			 * 
			 * @param prescaler
			 * 		The prescaler specifies by which factor the system clock
			 * 		will be divided.
			 */
			static inline void
			setPrescaler(const Prescaler prescaler)
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				ADC->CCR &= ~(0b11 << 17);				// Clear prescaler
				ADC->CCR |= prescaler << 17;		// set prescaler
#elif defined(STM32F10X)
				RCC->CFGR &= ~(0b11 << 14);			// Clear prescaler
				RCC->CFGR |= prescaler << 14;		// set prescaler
#endif
			}
			
#if defined(TEMPERATURE_REFVOLTAGE_AVIALABLE) || defined(__DOXYGEN__)
			/** Switch on temperature- and V_REF measurement. */
			static inline void
			enableTemperatureRefVMeasurement(void)
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				ADC->CCR |= ADC_CCR_TSVREFE;
#elif defined(STM32F10X)
				ADC1->CR2 |= ADC_CR2_TSVREFE;
#endif
			}

			/** Switch on temperature- and V_REF measurement. */
			static inline void
			disableTemperatureRefVMeasurement(void)
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				ADC->CCR &= ~ADC_CCR_TSVREFE;
#elif defined(STM32F10X)
				ADC1->CR2 &= ~ADC_CR2_TSVREFE;
#endif
			}
#endif // TEMPERATURE_REFVOLTAGE_AVIALABLE

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
					ADC1->CR2 |= ADC_CR2_ALIGN;
				}
				else {
					ADC1->CR2 &= ~ADC_CR2_ALIGN;
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
			setChannel(const Channels channel, const SampleTime sampleTime=static_cast<SampleTime>(0b000));

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
			enableFreeRunningMode(void)
			{
				ADC1->CR2 |= ADC_CR2_CONT;	// set to continuous mode
			}

			/**
			 * Disables free running mode
			 *
			 * The ADC will do only one sample and stop. The result will be in 
			 * the ADC register.
			 * 
			 * @pre The ADC clock must be started and the ADC switched on with 
			 * 		initialize()
			 */
			static inline void
			disableFreeRunningMode(void)
			{
				ADC1->CR2 &= ~ADC_CR2_CONT;		// set to single mode
			}

			/**
			 * Returns if the specified interrupt flag is set.
			 * 
			 * @return \c true if the flag is set
			 * @pre The ADC clock must be started and the ADC switched on with 
			 * 	initialize()
			 * @param flag The interrupt flag, which shall be checked.
			 */
			static inline bool
			isInterruptFlagSet(const Interrupt flag)
			{
				return ADC1->SR & flag;
			}

			/**
			 * Clears the specified interrupt flag.
			 *
			 * @param flag
			 * 		The interrupt flag, which shall be cleared.
			 *
			 * @pre The ADC clock must be started and the ADC switched on with 
			 * 		initialize().
			 */
			static inline void
			resetInterruptFlags(const Interrupt flag)
			{
				ADC1->SR &= ~flag;
			}

			/**
			 * Disables the ADC Conversion Complete Interrupt.
			 */
			static void
			disableInterrupt(const Interrupt interrupt);

			/**
			 * Enables the ADC Conversion Complete Interrupt.
			 *
			 * You could catch the interrupt using this example function:
			 * \li for STM32F4XX: \code extern "C" void ADC_IRQHandler() \endcode
			 * \li for STM32F10X: \code extern "C" void ADC1_2_IRQHandler() \endcode
			 * 
			 * @pre The ADC clock must be started and the ADC switched on with 
			 * 	initialize()
			 * 
			 * @param priority Priority to set
			 * @param interrupt The interrupt, which shall be enabled. See 
			 * 	Interrupt for available interrupts.
			 * 
			 * @note ADC1 and ADC2 interrupts are mapped onto the same interrupt
			 * 	vector. ADC3 interrupts are mapped onto a separate interrupt 
			 * 	vector.
			 */
			static void
			enableInterrupt(const Interrupt interrupt, const uint32_t priority);
			
			/**
			 * Turns off the ADC and its clock.
			 */
			static inline void
			shutdownAdc(void)
			{
				ADC1->CR2 &= ~(ADC_CR2_ADON);		// switch off ADC
				RCC->APB2ENR &= ~RCC_APB2ENR_ADC1EN; // stop ADC Clock
			}

			/**
			 * Start a new conversion or continuous conversions.
			 * 
			 * @pre A ADC channel must be selected with setChannel(). When using
			 * 	a STM32F10x a delay of at least t_STAB after initialize() must 
			 * 	be waited! 
			 * 
			 * @post The result can be fetched with getValue()
			 * @attention When using a STM32F10x, the application should allow a delay of t_STAB between
			 * 	power up and start of conversion. Refer to Reference Manual 
			 * 	(RM0008) ADC_CR2_ADON. 
			 */
			static inline void
			startConversion(void)
			{
#if defined(STM32F2XX) || defined(STM32F4XX)
				resetInterruptFlags(static_cast<Interrupt>(
						END_OF_CONVERSION_REGULAR | END_OF_CONVERSION_INJECTED |
						ANALOG_WATCHDOG | OVERRUN));
#elif defined(STM32F10X)
				resetInterruptFlags(static_cast<Interrupt>(
						END_OF_CONVERSION_REGULAR | END_OF_CONVERSION_INJECTED |
						ANALOG_WATCHDOG));
				
				// select the SWSTART event used to trigger the start of
				// conversion of a regular group
				ADC1->CR2 |= ADC_CR2_EXTTRIG | ADC_CR2_EXTSEL_0 |
						ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2;
#endif
				// starts single conversion for the regular group
				ADC1->CR2 |= ADC_CR2_SWSTART;
			}

			/** 
			 * @return If the conversion is finished.
			 * @pre A conversion should have been started with startConversion()
			 */
			static inline bool
			isConversionFinished(void)
			{
				return (ADC1->SR & ADC_SR_EOC);
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
				return ADC1->DR;
			}
		};

#elif defined(STM32F3XX)
/* id < 4 */

		/**
		 * Analog/Digital-Converter module (ADC1).
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
		class Adc1 : public Interface
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
				PinA0 =  1,
				PinA1 =  2,
				PinA2 =  3,
				PinA3 =  4,
				PinF4 =  5,
				PinC0 =  6,
				PinC1 =  7,
				PinC2 =  8,
				PinC3 =  9,
				PinF2 = 10,
				// 11-14 reserved
				Channel15 = 15,
				Opamp1    = 15,
				Channel16 = 15,
				VTS       = 16,
				BatDiv2   = 17,
				InternalReference = 18,
				// TODO: Add internal connections
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
				Div1 				= RCC_CFGR2_ADCPRE12_DIV1,
				Div2 				= RCC_CFGR2_ADCPRE12_DIV2,
				Div4 				= RCC_CFGR2_ADCPRE12_DIV4,
				Div6 				= RCC_CFGR2_ADCPRE12_DIV6,
				Div8 				= RCC_CFGR2_ADCPRE12_DIV8,
				Div10 				= RCC_CFGR2_ADCPRE12_DIV10,
				Div12 				= RCC_CFGR2_ADCPRE12_DIV12,
				Div16 				= RCC_CFGR2_ADCPRE12_DIV16,
				Div32 				= RCC_CFGR2_ADCPRE12_DIV32,
				Div64 				= RCC_CFGR2_ADCPRE12_DIV64,
				Div128 				= RCC_CFGR2_ADCPRE12_DIV128,
				Div256 				= RCC_CFGR2_ADCPRE12_DIV256,
				Div256AllBits 		= RCC_CFGR2_ADCPRE12,	// for bit clear
				
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
				ADC1->CR |= ADC_CR_ADDIS;
				if (blocking) {
					// wait for ADC_CR_ADDIS to be cleared by hw
					while(ADC1->CR & ADC_CR_ADDIS);
				}
				// disable clock
				RCC->AHBENR &= ~RCC_AHBENR_ADC12EN;
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
					ADC1->CR |= ADC_CR_ADCAL |
													static_cast<uint32_t>(mode);
					if(blocking) {
						// wait for ADC_CR_ADCAL to be cleared by hw
						while(ADC1->CR & ADC_CR_ADCAL);
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
					ADC1->CFGR |= ADC_CFGR_ALIGN;
				}
				else {
					ADC1->CFGR &= ~ADC_CFGR_ALIGN;
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
				ADC1->SQR1 = (static_cast<uint8_t>(channel) & 0b11111) << 6;

				if (static_cast<uint8_t>(channel) < 10) {
					tmpreg = ADC1->SMPR1
						& ((~ADC_SMPR1_SMP0) << (static_cast<uint8_t>(channel) * 3));
					tmpreg |= static_cast<uint32_t>(sampleTime) <<
									(static_cast<uint8_t>(channel) * 3);
					ADC1->SMPR1 = tmpreg;
				}
				else {
					tmpreg = ADC1->SMPR2
						& ((~ADC_SMPR2_SMP10) << ((static_cast<uint8_t>(channel)-10) * 3));
					tmpreg |= static_cast<uint32_t>(sampleTime) <<
									((static_cast<uint8_t>(channel)-10) * 3);
					ADC1->SMPR2 = tmpreg;
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
					ADC1->CFGR |=  ADC_CFGR_CONT; // set to continuous mode
				} else {
					ADC1->CFGR &= ~ADC_CFGR_CONT; // set to single mode
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
				ADC1->CR |= ADC_CR_ADSTART;
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
				return ADC1->DR;
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
				const IRQn_Type INTERRUPT_VECTOR = ADC1_2_IRQn;
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
				ADC1->IER |= static_cast<uint32_t>(interrupt);
			}

			static inline void
			disableInterrupt(const Interrupt interrupt)
			{
				ADC1->IER &= ~static_cast<uint32_t>(interrupt);
			}

			static inline InterruptFlag
			getInterruptFlags()
			{
				return static_cast<InterruptFlag>(ADC1->ISR);
			}

			static inline void
			resetInterruptFlags(const InterruptFlag flags)
			{
				// Flags are cleared by writing a one to the flag position.
				// Writing a zero is ignored.
				ADC1->ISR = static_cast<uint32_t>(flags);
			}
		};

		ENUM_CLASS_FLAG(Adc1::InterruptFlag)
#else
	#  error "Unknown CPU Type. Please define STM32F10X_.., \
											STM32F2XX, STM32F3XX or STM32F4XX"
#endif
	}

}

#endif	// XPCC_STM32__ADC1_HPP
