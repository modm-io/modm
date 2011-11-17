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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_STM32__GENERAL_PURPOSE_TIMER_HPP
#define XPCC_STM32__GENERAL_PURPOSE_TIMER_HPP

#include <stdint.h>
#include "../device.h"

namespace xpcc
{
	namespace stm32
	{
		class GeneralPurposeTimer
		{
		public:
			enum Interrupt
			{
				UPDATE_INTERRUPT = TIM_DIER_UIE,
				CAPTURE_COMPARE_1_INTERRUPT = TIM_DIER_CC1IE,
				CAPTURE_COMPARE_2_INTERRUPT = TIM_DIER_CC2IE,
				CAPTURE_COMPARE_3_INTERRUPT = TIM_DIER_CC3IE,
				CAPTURE_COMPARE_4_INTERRUPT = TIM_DIER_CC4IE,
				COM_INTERRUPT = TIM_DIER_COMIE,
				TRIGGER_INTERRUPT = TIM_DIER_TIE,
				BREAK_INTERRUPT = TIM_DIER_BIE,
				UPDATE_DMA_REQUEST_INTERRUPT = TIM_DIER_UDE,
				CAPTURE_COMPARE_1_DMA_REQUEST_INTERRUPT = TIM_DIER_CC1DE,
				CAPTURE_COMPARE_2_DMA_REQUEST_INTERRUPT = TIM_DIER_CC2DE,
				CAPTURE_COMPARE_3_DMA_REQUEST_INTERRUPT = TIM_DIER_CC3DE,
				CAPTURE_COMPARE_4_DMA_REQUEST_INTERRUPT = TIM_DIER_CC4DE,
				COM_DMA_REQUEST_INTERRUPT = TIM_DIER_COMDE,
				TRIGGER_DMA_REQUEST_INTERRUPT = TIM_DIER_TDE,
			};

			enum Mode
			{
				MODE_UPCOUNTER,
				MODE_OUTPUT_COMPARE,
				MODE_PWM,
				MODE_PWM_CENTER_ALIGNED,
				MODE_ONE_SHOOT,
				MODE_ENCODER,
				MODE_INPUT_CAPTURE,
			};
		};
	}
}

#endif // XPCC_STM32__GENERAL_PURPOSE_TIMER_HPP
