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

#ifndef XPCC_I2C__DELEGATE_HPP
#define XPCC_I2C__DELEGATE_HPP

#include <cstddef>
#include <stdint.h>

#include <xpcc/driver/interface.hpp>
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
		 * Be aware the methods may or may not be called during the I2C
		 * interrupt even before returning from initiating function.
		 * This Delegate will stay attached to I2C during whole operation.
		 *
		 * \author Georgi Grinshpun
		 * \author Niklas Hauser
		 * \ingroup	interface
		 */
		class Delegate : public Interface
		{
		public:
			/**
			 * Possible operations after start or restart.
			 */
			enum NextOperation
			{
				STOP_OP,
				RESTART_OP,
				WRITE_OP,
				READ_OP,
			};
			
			/**
			 * Possible operations after write.
			 */
			enum NextOperationAfterWrite
			{
				WRITE_STOP,
				WRITE_RESTART,
				WRITE_WRITE,
			};
			
			/**
			 * Possible operations after read.
			 */
			enum NextOperationAfterRead
			{
				READ_STOP,
				READ_RESTART,
			};
			
			
			struct Starting {
				NextOperation next;
				uint8_t address;
			};
			
			struct Reading {
				NextOperationAfterRead next;
				uint8_t *buffer;
				std::size_t size;
			};
			
			struct Writing {
				NextOperationAfterWrite next;
				const uint8_t *buffer;
				std::size_t size;
			};
			
			enum DetachCause
			{
				NORMAL_STOP,
				ERROR_CONDITION,
				SOFTWARE_RESET
			};
			
			/*
			 * Is called one time while attaching signaling this module
			 * that it is being attached to the driver.
			 * \return if this delegate is able to be attached. Returning
			 * false the attaching will not be performed. After returning
			 * true, this delegate will be attached and stay so until stop
			 * is called.
			 */
			virtual bool
			attaching() = 0;

			/*
			 * Is called at least one time after attaching returned true. Will
			 * also be called while restart.
			 */
			virtual Starting
			started() = 0;

			/**
			 * Before a read operation starts is called to retrieve the buffer
			 * where incoming data to place to.
			 */
			virtual Reading
			reading() = 0;

			/**
			 * Before a write operation starts is called to retrieve buffer
			 * which contains the data to write.
			 */
			virtual Writing
			writing() = 0;

			/**
			 * Is called just before (or just after) this delegate is detached from i2c.
			 */
			virtual void
			stopped(DetachCause cause) = 0;
		};
	}
}

#endif // XPCC_I2C__DELEGATE_HPP
