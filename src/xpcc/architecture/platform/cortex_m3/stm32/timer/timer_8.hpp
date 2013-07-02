// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file. 
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__TIMER_8_HPP
#define XPCC_STM32__TIMER_8_HPP

#include "timer_base.hpp"

namespace xpcc
{
	namespace stm32
	{
		/**
		 * @brief		Advanced Control Timer 8
		 * 
		 * 
		 * Interrupt handler for High Density:
		 * - TIM8_BRK_IRQHandler
		 * - TIM8_UP_IRQHandler
		 * - TIM8_TRG_COM_IRQHandler
		 * - TIM8_CC_IRQHandler
		 * 
		 * Interrupt handler for XL Density:
		 * - TIM8_BRK_TIM12_IRQn
		 * - TIM8_UP_TIM13_IRQn
		 * - TIM8_TRG_COM_TIM14_IRQn
		 * - TIM8_CC_IRQn
		 * 
		 * 
		 * Example:
		 * \code
		 * extern "C" void
		 * TIM8_UP_IRQHandler(void)
		 * {
		 *     Timer8::resetInterruptFlags(Timer8::...);
		 *     
		 *     ...
		 * }
		 * \endcode
		 * 
		 * \warning	The Timer has much more possibilities than presented by this
		 * 			interface (e.g. Input Capture, Trigger for other Timers, DMA).
		 * 			It might be expanded in the future.
		 * 
		 * @author		Fabian Greif
		 * @ingroup		stm32
		 */
		class Timer8 : public AdvancedControlTimer
		{
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
		public:
			
#else
		
#endif
			
		public:
			enum MasterMode
			{
				MASTER_RESET 			= 0,							// 0b000
				MASTER_ENABLE 			= TIM_CR2_MMS_0,				// 0b001
				MASTER_UPDATE 			= TIM_CR2_MMS_1,				// 0b010
				MASTER_COMPARE_PULSE 	= TIM_CR2_MMS_1 | TIM_CR2_MMS_0,// 0b011
				MASTER_COMPARE_OC1REF 	= TIM_CR2_MMS_2,				// 0b100
				MASTER_COMPARE_OC2REF 	= TIM_CR2_MMS_2 | TIM_CR2_MMS_0,// 0b101
				MASTER_COMPARE_OC3REF 	= TIM_CR2_MMS_2 | TIM_CR2_MMS_1,// 0b110
				MASTER_COMPARE_OC4REF 	= TIM_CR2_MMS_2 | TIM_CR2_MMS_1	// 0b111
														| TIM_CR2_MMS_0,
			};

			enum CaptureCompareControlUpdate
			{
				SET_COMG = 0,
				SET_COMG_OR_TRGI_RIDING_EDGE = TIM_CR2_CCUS
			};

#if defined(STM32F3XX)
			enum MasterMode2
			{
				MASTER2_RESET			= 0,							//0b0000
				MASTER2_ENABLE			= TIM_CR2_MMS2_0,				//0b0001
				MASTER2_UPDATE			= TIM_CR2_MMS2_1,				//0b0010
				MASTER2_COMPARE_PULSE	= TIM_CR2_MMS2_1				//0b0010
										| TIM_CR2_MMS2_0,
				MASTER2_COMPARE_OC1REF	= TIM_CR2_MMS2_2,				//0b0100
				MASTER2_COMPARE_OC2REF	= TIM_CR2_MMS2_2				//0b0101
										| TIM_CR2_MMS2_0,
				MASTER2_COMPARE_OC3REF	= TIM_CR2_MMS2_2				//0b0110
										| TIM_CR2_MMS2_1,
				MASTER2_COMPARE_OC4REF	= TIM_CR2_MMS2_2				//0b0111
										| TIM_CR2_MMS2_1 | TIM_CR2_MMS2_0,
				MASTER2_COMPARE_OC5REF	= TIM_CR2_MMS2_3,				//0b1000
				MASTER2_COMPARE_OC6REF	= TIM_CR2_MMS2_3				//0b1001
										| TIM_CR2_MMS2_0,
				// TODO: Add other Master Modes
			};
#endif /* defined(STM32F3XX) */

			enum SlaveModeTrigger
			{
				TRIGGER_INTERNAL_0 = 0,
				TRIGGER_INTERNAL_1 = TIM_SMCR_TS_0,
				TRIGGER_INTERNAL_2 = TIM_SMCR_TS_1,
				TRIGGER_INTERNAL_3 = TIM_SMCR_TS_1 | TIM_SMCR_TS_0,
				TRIGGER_TI1_EDGE_DETECTOR = TIM_SMCR_TS_2,
				TRIGGER_FILTERED_TI1 = TIM_SMCR_TS_2 | TIM_SMCR_TS_0,
				TRIGGER_FILTERED_TI2 = TIM_SMCR_TS_2 | TIM_SMCR_TS_1,
				TRIGGER_EXTERNAL = TIM_SMCR_TS_2 | TIM_SMCR_TS_1 | TIM_SMCR_TS_0,
			};
			
			enum SlaveMode
			{
				SLAVE_DISABLED	= 0, // Slave mode disabled - if CEN = '1' then the prescaler is clocked directly by the internal clock.
				SLAVE_ENCODER_1	= TIM_SMCR_SMS_0, // Counter counts up/down on TI2FP2 edge depending on TI1FP1 level.
				SLAVE_ENCODER_2	= TIM_SMCR_SMS_1, // Counter counts up/down on TI1FP1 edge depending on TI2FP2 level.
				SLAVE_ENCODER_3	= TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0, // Counter counts up/down on both TI1FP1 and TI2FP2 edges depending on the level of the other input.
				SLAVE_RESET		= TIM_SMCR_SMS_2, // Rising edge of the selected trigger input (TRGI) reinitializes the counter and generates an update of the registers.
				SLAVE_GATED		= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_0, // The counter clock is enabled when the trigger input (TRGI) is high. The counter stops (but is not reset) as soon as the trigger becomes low. Both start and stop of the counter are controlled.
				SLAVE_TRIGGER	= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1, // The counter starts at a rising edge of the trigger TRGI (but it is not reset). Only the start of the counter is controlled.
				SLAVE_EXTERNAL_CLOCK = TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_SMS_0, // Rising edges of the selected trigger (TRGI) clock the counter.
			};

			enum OffStateForRunMode
			{
				OSSR_DISABLE = 0,
				OSSR_ENABLE = TIM_BDTR_OSSR,
			};

			enum OffStateForIdleMode
			{
				OSSI_DISABLE = 0,
				OSSI_ENABLE = TIM_BDTR_OSSI,
			};

			enum OutputIdleState
			{
				IDLE_STATE_RESET = 0,
				IDLE_STATE_SET = TIM_CR2_OIS1,
			};

			enum Event
			{
			#if defined(STM32F3XX)
				EVENT_BREAK_2 							= TIM_EGR_B2G,
			#endif
				EVENT_BREAK 							= TIM_EGR_BG,
				EVENT_TRIGGER 							= TIM_EGR_TG,
				EVENT_CAPTURE_COMPARE_CONTROL_UPDATE 	= TIM_EGR_COMG,
				EVENT_CAPTURE_COMPARE_4 				= TIM_EGR_CC4G,
				EVENT_CAPTURE_COMPARE_3 				= TIM_EGR_CC3G,
				EVENT_CAPTURE_COMPARE_2 				= TIM_EGR_CC2G,
				EVENT_CAPTURE_COMPARE_1 				= TIM_EGR_CC1G,
				EVENT_UPDATE 							= TIM_EGR_UG,
			};

			
			static void
			enable();
			
			static void
			disable();
			
			static inline void
			pause()
			{
				TIM8->CR1 &= ~TIM_CR1_CEN;
			}
			
			static inline void
			start()
			{
				TIM8->CR1 |= TIM_CR1_CEN;
			}
			
			static void
			setMode(Mode mode,
					SlaveMode slaveMode = SLAVE_DISABLED,
					SlaveModeTrigger slaveModeTrigger = TRIGGER_INTERNAL_0,
					MasterMode masterMode = MASTER_RESET
#if defined(STM32F3XX)
					, MasterMode2 masterMode2 = MASTER2_RESET
#endif /* defined(STM32F3XX) */
					);

			static inline void
			setCaptureCompareControlUpdate(CaptureCompareControlUpdate update)
			{
				TIM8->CR2 =  (TIM8->CR2 & ~TIM_CR2_CCUS)
									| update;
			};


			/*
			 * Enables Capture/Compare preloaded control
			 *
			 * If enabled CCxE, CCxNE and OCxM bits are preloaded and only
			 * updated when the COMG bit is set or on a rising edge on TRGI
			 * This is determined by the CaptureCompareControlUpdate setting.
			 */
			static inline void
			enableCaptureComparePreloadedControl
			(CaptureCompareControlUpdate update = SET_COMG)
			{
				TIM8->CR2 = (TIM8->CR2 & ~TIM_CR2_CCUS)
									| update | TIM_CR2_CCPC;
			}

			static inline void
			disableCaptureComparePreloadedControl()
			{
				TIM8->CR2 &= ~TIM_CR2_CCPC;
			}
			
			static inline void
			setPrescaler(uint16_t prescaler)
			{
				// Because a prescaler of zero is not possible the actual
				// prescaler value is \p prescaler - 1 (see Datasheet)
				TIM8->PSC = prescaler - 1;
			}
			
			static inline void
			setOverflow(uint16_t overflow)
			{
				TIM8->ARR = overflow;
			}
			
			static uint16_t
			setPeriod(uint32_t microseconds, bool autoApply = true);
			
			static inline void
			applyAndReset()
			{
				// Generate Update Event to apply the new settings for ARR
				generateEvent(EVENT_UPDATE);
			}

			static inline void
			generateEvent(Event ev)
			{
				TIM8->EGR |= static_cast<uint32_t>(ev);
			}

			static inline uint16_t
			getValue()
			{
				return TIM8->CNT;
			}
			
			static inline void
			setValue(uint16_t value)
			{
				TIM8->CNT = value;
			}
			
			static inline void
			enableOutput()
			{
				TIM8->BDTR |= TIM_BDTR_MOE;
			}

			/*
			 * Enable/Disable automatic set of MOE bit at the next update event
			 */
			static inline void
			setAutomaticUpdate(bool enable)
			{
				if(enable)
					TIM8->BDTR |= TIM_BDTR_AOE;
				else
					TIM8->BDTR &= ~TIM_BDTR_AOE;
			}

			static inline void
			setOffState(OffStateForRunMode runMode, OffStateForIdleMode idleMode)
			{
				uint32_t flags = TIM8->BDTR;
				flags &= ~(TIM_BDTR_OSSR | TIM_BDTR_OSSI);
				flags |= runMode | idleMode;
				TIM8->BDTR = flags;
			}

			static inline void
			setOutputIdleState(uint32_t channel, OutputIdleState idle,
									OutputIdleState idle_n = IDLE_STATE_RESET)
			{
				uint32_t flags = TIM8->CR2;
				channel -= 1;
				flags &=  (IDLE_STATE_SET << (channel * 2))
						| (IDLE_STATE_SET << (channel * 2 + 1));
				flags |= (idle   << (channel * 2));
				flags |= (idle_n << (channel * 2 + 1));
				TIM8->CR2 = flags;
			}

			/*
			 * Set Dead Time Value
			 *
			 * Different Resolution Depending on DeadTime[7:5]:
			 *     0xx =>  DeadTime[6:0]            * T(DTS)
			 *     10x => (DeadTime[5:0] + 32) *  2 * T(DTS)
			 *     110 => (DeadTime[4:0] + 4)  *  8 * T(DTS)
			 *     111 => (DeadTime[4:0] + 2)  * 16 * T(DTS)
			 */
			static inline void
			setDeadTime(uint8_t deadTime)
			{
				uint32_t flags = TIM8->BDTR;
				flags &= TIM_BDTR_DTG;
				flags |= deadTime;
				TIM8->BDTR = flags;
			}

			/*
			 * Set Dead Time Value
			 *
			 * Different Resolution Depending on DeadTime[7:5]:
			 *     0xx =>  DeadTime[6:0]            * T(DTS)
			 *     10x => (DeadTime[5:0] + 32) *  2 * T(DTS)
			 *     110 => (DeadTime[4:0] + 4)  *  8 * T(DTS)
			 *     111 => (DeadTime[4:0] + 2)  * 16 * T(DTS)
			 */
			static inline void
			setDeadTime(DeadTimeResolution resolution, uint8_t deadTime)
			{
				uint8_t bitmask;
				switch(resolution){
					case FROM_0_125NS_STEP:
						bitmask = 0b01111111;
						break;
					case FROM_16US_250NS_STEP:
						bitmask = 0b00111111;
						break;
					case FROM_32US_1US_STEP:
					case FROM_64US_2US_STEP:
						bitmask = 0b00011111;
						break;
					default:
						bitmask = 0x00;
						break;
				}
				uint32_t flags = TIM8->BDTR;
				flags &= TIM_BDTR_DTG;
				flags |= (deadTime & bitmask) | resolution;
				TIM8->BDTR = flags;
			}

		public:
			static void
			configureInputChannel(uint32_t channel, InputCaptureMapping input,
					InputCapturePrescaler prescaler,
					InputCapturePolarity polarity, uint8_t filter,
					bool xor_ch1_3=false);

			static void
			configureOutputChannel(uint32_t channel, OutputCompareMode mode,
					uint16_t compareValue);
			// TODO: Maybe add some functionality from the configureOutput
			//       function below...

			/*
			 * Configure Output Channel without changing the Compare Value
			 *
			 * Normally used to REconfigure the Output channel without touching
			 * the compare value. This can e.g. be useful for commutation of a
			 * bldc motor.
			 *
			 * This function probably won't be used for a one time setup but
			 * rather for adjusting the output setting periodically.
			 * Therefore it aims aims to provide the best performance possible
			 * without sacrificing code readability.
			 */
			static void
			configureOutputChannel(uint32_t channel, OutputCompareMode mode,
					PinState out, OutputComparePolarity polarity,
					PinState out_n,
					OutputComparePolarity polarity_n = OUTPUT_ACTIVE_HIGH,
					OutputComparePreload preload = PRELOAD_REGISTER_DISABLED);

			/*
			 * Configure Output Channel width Mode/OutputPort uint
			 *
			 * This is the least typesafe way of doing this and should only
			 * be done if it provides a necessary performance
			 * (or more or less) laziness benefit.
			 * i.e. if you have specific mode/output uints precalculated and
			 * just want to load them as fast as possible.
			 *
			 * The "mode/output" uint contains four bits
			 * that describe the intended output setting:
			 * Bit0: Output Enabled/Disabled
			 * Bit1: Output Polarity
			 * Bit2: Complementary Output Enable/Disable
			 * Bit3: Complementary Output Polarity
			 *
			 * As well as Mode Information (Bit6-Bit4)
			 * which is just an OutputCompareMode constant ored with the
			 * port output quadruple specified above.
			 */
			static void
			configureOutputChannel(uint32_t channel, uint32_t modeOutputPorts);

			static inline void
			setCompareValue(uint32_t channel, uint16_t value)
			{
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
				*(&TIM8->CCR1 + (channel - 1)) = value;
#else
				*(&TIM8->CCR1 + ((channel - 1) * 2)) = value;
#endif
			}
			
			static inline uint16_t
			getCompareValue(uint32_t channel)
			{
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
				return *(&TIM8->CCR1 + (channel - 1));
#else
				return *(&TIM8->CCR1 + ((channel - 1) * 2));
#endif
			}
			
		public:
			/**
			 * Enables or disables Interrupt Vectors.
			 * 
			 * You must pass an or-conjuncted list of entries from the
			 * Interrupt enum. Interrupt vectors which fit to 
			 * will be enabled (or disabled if \p enable = false).
			 * 
			 * The Adanvced timers have four interrupt vectors:
			 * - UP (used by INTERRUPT_UPDATE)
			 * - BRK (used by INTERRUPT_BREAK)
			 * - TRG_COM (used by INTERRUPT_TRIGGER and INTERRUPT_COM)
			 * - CC (used by INTERRUPT_CAPTURE_COMPARE_1..3)
			 */
			static void
			enableInterruptVector(Interrupt interrupt, bool enable, uint32_t priority);
			
			static inline void
			enableInterrupt(Interrupt interrupt)
			{
				TIM8->DIER |= interrupt;
			}
			
			static inline void
			disableInterrupt(Interrupt interrupt)
			{
				TIM8->DIER &= ~interrupt;
			}
			
			static inline void
			enableDmaRequest(DmaRequestEnable dmaRequests)
			{
				TIM8->DIER |= dmaRequests;
			}
			
			static inline void
			disableDmaRequest(DmaRequestEnable dmaRequests)
			{
				TIM8->DIER &= ~dmaRequests;
			}
			
			static inline InterruptFlag
			getInterruptFlags()
			{
				return (InterruptFlag) TIM8->SR;
			}
			
			static inline void
			resetInterruptFlags(InterruptFlag flags)
			{
				// Flags are cleared by writing a zero to the flag position.
				// Writing a one is ignored.
				TIM8->SR = ~flags;
			}
		};
	}
}

#endif // XPCC_STM32__TIMER_8_HPP