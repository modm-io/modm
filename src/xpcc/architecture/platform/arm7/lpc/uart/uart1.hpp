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

#ifndef XPCC__UART1_HPP
#define XPCC__UART1_HPP

namespace xpcc
{
	/**
	 * @brief	Driver for UART1 of the LPC2468
	 * 
	 * To use this class for other LPC2xxx device you need to check the PINSELx
	 * and PINMODEx registers to match the location of the UARTs.
	 * 
	 * This implementation requires the VIC to be active.
	 * 
	 * @date	May 2011
	 * @author	Fabian Greif 
	 */
	class Uart1
	{
	public:
		/**
		 * @brief	Initialize the hardware
		 * 
		 * Sets the registers etc.
		 * 
		 * With the trigger level it is possible to make a tradeoff between
		 * latency and interrupt count. A small value will give you more interrupts
		 * but a faster response time, a bigger value will decrease the
		 * number of interrupts.
		 * Nonetheless after 3.5 - 4.5 character times the remaining bytes will
		 * be collected from the FIFO. 
		 * 
		 * Possible values for \c level:
		 * - 0 => 1 byte
		 * - 1 => 4 byte
		 * - 2 => 8 byte
		 * - 3 => 14 byte
		 * 
		 * @brief	level	Trigger level for the UART Fifos.
		 */
		static void
		initialize(uint8_t level = 2);
		
		/**
		 * @brief	Send a character
		 * 
		 * @param	c			character to be send
		 * @param	blocking	If \c true the function will wait until the
		 * 						character could be send. Otherwise the function
		 * 						will return immediately even if sending failed.
		 * @return	\c true if the character was send, \c false otherwise.
		 */
		static bool
		write(char c, bool blocking = true);
		
		/**
		 * @brief	Check if a character was received and is ready to be read
		 */
		static bool
		isCharacterAvailable();
		
		/**
		 * @brief	Read a single character
		 * 
		 * @return	\c true if a character was available and was stored in \c c,
		 * 			false if nothing was available. \c c will remain untouched
		 * 			in this case.
		 */
		static bool
		read(char &c, bool blocking = false);
	};
}

#endif // XPCC__UART1_H