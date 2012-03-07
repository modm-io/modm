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
 * $Id: arm7.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ARM7_HPP
#define XPCC__ARM7_HPP

/**
 * \ingroup		platform
 * \defgroup	arm7	ARM7TDMI
 * 
 * Tested with Codesourcery G++ lite.
 * 
 * <h2>Configuration</h2>
 * 
 * The following defines are available:
 * - \c ARM7_ENABLE_ABORT_HANDLER set to \c 1 to enable a handler for data and
 *   prefetch abort and undefined instruction conditions. This handler uses
 *   the xpcc::log::error stream for output, which needs to be a \b none
 *   interrupt-driven output stream (for example xpcc::at91::Debug).
 *   During the execution of the abort handler interrupts are disabled.
 *   
 *   The default handler without output just enters an infinite loop.
 * 
 *    
 */

#if defined __ARM_LPC2000__
#	include "arm7/lpc.hpp"
#elif defined __ARM_AT91__
#	include "arm7/at91.hpp"
#endif

#endif	// XPCC__ARM7_HPP
