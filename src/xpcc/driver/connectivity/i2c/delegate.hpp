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
 *
 * $Id: delegate.hpp 736 2012-02-21 22:06:09Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C__DELEGATE_HPP
#define XPCC_I2C__DELEGATE_HPP

#include <cstddef>
#include <stdint.h>

//#include <xpcc/driver/interface.hpp>
#include "constants.hpp"

namespace xpcc
{
	/**
	 * \brief	I2C Interface
	 * \ingroup	i2c
	 */
	namespace i2c
	{
		/**
		 * \brief	Abstract class for delegation
		 *
		 * For true asynchronous operation, the communication driver should
		 * inherit from this class, allowing multistage driver design and
		 * performance gain by premature data evaluation.
		 * The methods will be called during the TWI interrupt after completion,
		 * a restart or error condition.
		 * By using BusState, BusyState, ErrorState and parameter information,
		 * further operation can be derived.
		 * Be aware that the delegate object is reset before calling a method,
		 * so reattaching the delegate object is necessary if further callbacks
		 * are needed.
		 * At maximum one vTable lookup is done per operation, making
		 * polymorphism an efficient callback solution in this case.
		 * Methods must be overwritten to disable their default behavior.
		 *
		 * \author Niklas Hauser
		 * \ingroup	interface
		 */
		class Delegate //: public Interface
		{
		public:
			/**
			 * \brief	This method is called in the interrupt at
			 *			the successful completion of an action (read/write).
			 *			Depending on BusState, further operations can now be
			 *			started.
			 * 
			 * \param	data	pointer to data buffer
			 * \param	index	number of bytes written or read
			 * \param	reading	\c true if last operation was reading
			 */
			virtual void
			twiCompletion(const uint8_t *data, std::size_t index, bool reading);
			
			/**
			 * \brief	This method is called during the interrupt at error
			 *			occurance.
			 */
			virtual void
			twiError(ErrorState error);
		};
	}
}

#endif // XPCC_I2C__DELEGATE_HPP
