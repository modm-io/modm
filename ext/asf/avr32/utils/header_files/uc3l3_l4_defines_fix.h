/**
 * \file
 *
 * \brief Missing Toolchain Definition for UC3 L3 and UC3 L4 header files
 *
 * Copyright (c) 2012 Atmel Corporation. All rights reserved.
 * 
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _UCL3_L4_DEFINES_FIX_
#define _UCL3_L4_DEFINES_FIX_

#include <parts.h>

#if (UC3L3_L4)

//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#ifndef AVR32_PM_CPU_MAX_FREQ
#define AVR32_PM_CPU_MAX_FREQ              50000000
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif // AVR32_PM_CPU_MAX_FREQ

#ifndef AVR32_PM_HSB_MAX_FREQ
#define AVR32_PM_HSB_MAX_FREQ              50000000
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif // AVR32_PM_HSB_MAX_FREQ

#ifndef AVR32_PM_PBA_MAX_FREQ
#define AVR32_PM_PBA_MAX_FREQ              50000000
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif // AVR32_PM_PBA_MAX_FREQ

#ifndef AVR32_PM_PBB_MAX_FREQ
#define AVR32_PM_PBB_MAX_FREQ              50000000
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif  // AVR32_PM_PBB_MAX_FREQ

#ifndef AVR32_PM_MCSEL_DFLL0
#define AVR32_PM_MCSEL_DFLL0 2
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif  // AVR32_PM_MCSEL_DFLL0

#ifndef AVR32_PM_MCSEL_OSC0
#define AVR32_PM_MCSEL_OSC0 1
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif  // AVR32_PM_MCSEL_OSC0

#ifndef AVR32_PM_MCSEL_RC120M
#define AVR32_PM_MCSEL_RC120M 3
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif  // AVR32_PM_MCSEL_RC120M

#ifndef AVR32_PM_MCSEL_SLOW
#define AVR32_PM_MCSEL_SLOW 0
#else
#warning "Duplicate define(s) to remove from the ASF"
#endif // AVR32_PM_MCSEL_SLOW

#endif // end UC3L3_L4

#endif // _UCL3_L4_DEFINES_FIX
