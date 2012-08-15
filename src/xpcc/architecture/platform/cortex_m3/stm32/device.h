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

#ifndef STM32__DEVICE_H
#define STM32__DEVICE_H

/**
 * Reduce compile dependencies for the hardware drivers.
 * Do not include <xpcc/architecture.hpp> to avoid recompiling the whole set
 * of drivers if anything unrelated changed.
 */

#if !defined(STM32F4XX) && !defined(STM32F2XX) && \
	!defined(STM32F10X_CL) && \
	!defined(STM32F10X_LD) && !defined(STM32F10X_MD) && !defined(STM32F10X_HD) && !defined(STM32F10X_XL) && \
	!defined(STM32F10X_LD_VL) && !defined(STM32F10X_MD_VL) && !defined (STM32F10X_HD_VL) 
#	error "Please select the target STM32F??X device used in your application (in the stm32f??x.h file)"
#endif

#if defined(STM32F2XX)
#	include <stm32f2xx.h>
#elif defined(STM32F4XX)
#	include <stm32f4xx.h>
#else
#	include <stm32f10x.h>
#endif

/**
 * \defgroup	stm32f1		STM32F10x
 * \ingroup		stm32
 */

/**
 * \defgroup	stm32f2_4	STM32F2xx and STM32F4xx
 * \ingroup		stm32
 */

#endif // STM32__DEVICE_H
