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

#ifndef XPCC__ERROR_REPORT_HPP
#define XPCC__ERROR_REPORT_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * Global error reporter
	 * 
	 * Used to report severe errors at one place.
	 * 
	 * @ingroup	debug
	 * @author	Fabian Greif
	 */
	class ErrorReport
	{
	public:
		typedef void (*Handler)(uint16_t errorCode);
		
		/**
		 * Attach a Global Error Handler Function.
		 * 
		 * This handler is used to report severe errors (Buffer Overflows in
		 * Peripheral Drivers etc.). The error code is architecture dependent.
		 */
		static void
		attach(Handler handler);
		
		/**
		 * Remove Error Handler.
		 */
		static void
		detach();
		
		static inline void
		report(uint16_t errorCode)
		{
			globalErrorHandler(errorCode);
		}
		
	private:
		ErrorReport();
		
		static Handler globalErrorHandler;
	};
}

#endif	// XPCC__ERROR_REPORT_HPP
