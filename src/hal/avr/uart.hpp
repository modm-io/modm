// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef XPCC_UART_HPP
#define XPCC_UART_HPP

#include <stdint.h>

#include "../io/iodevice.hpp"

namespace xpcc
{
	//! \brief	UART baudrate expression
	//! 
	//! \param	fcpu		system clock in Mhz, e.g. 4000000L for 4Mhz
	//! \param	baudrate	baudrate in bps, e.g. 1200, 2400, 9600
	#define UART_BAUD_SELECT(baudRate,fcpu) ((fcpu)/((baudRate)*16l)-1)
	
	//! \brief	UART baudrate expression for ATmega double speed mode
	//! 
	//! \param	fcpu		system clock in Mhz, e.g. 4000000L for 4Mhz
	//! \param	baudrate	baudrate in bps, e.g. 1200, 2400, 9600
	#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,fcpu) (((fcpu)/((baudRate)*8l)-1)|0x8000)
	
	class Uart : public IODevice
	{
	public:
		/*static Uart&
		instance() {
			static Uart uart;
			return uart;
		}*/
		// TODO!
		Uart() {};
		
		void
		initialize(uint16_t baudrate);
		
		virtual void
		put(char c);
		
		virtual void
		put(const char* str) {
			IODevice::put(str);
		}
		
		virtual void
		flush() {
			//this->put('\n');
		}
		
		virtual bool
		get(char& c);
	
	private:
		//Uart() {};
		
		Uart(const Uart&);
		
		Uart&
		operator=(const Uart &);
	};
	
	//extern Uart uart;
}

#endif // XPCC_UART_HPP
