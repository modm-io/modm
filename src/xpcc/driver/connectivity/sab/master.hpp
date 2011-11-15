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

#ifndef XPCC_SAB__MASTER_HPP
#define XPCC_SAB__MASTER_HPP

#include <stdint.h>
#include <xpcc/workflow/timeout.hpp>

#include "interface.hpp"

namespace xpcc
{
	namespace sab
	{
		/**
		 * 
		 * Requires xpcc::Clock to be implemented.
		 * 
		 * \see	xpcc::Clock
		 * 
		 * \author	Fabian Greif
		 * \ingroup	sab
		 */
		template <typename Interface>
		class Master
		{
		public:
			static void
			initialize();
			
			/**
			 * \brief	Start a new query with a payload
			 * 
			 * \param slaveAddress	
			 * \param command		
			 * \param payload		
			 * \param responseLength	Expected payload length of the response
			 */
			template <typename T>
			static void
			query(uint8_t slaveAddress, uint8_t command,
					const T& payload, uint8_t responseLength);
			
			/**
			 * \brief	Start a new query without any payload
			 * 
			 */
			static void
			query(uint8_t slaveAddress, uint8_t command, uint8_t responseLength);
			
			static bool
			isQueryCompleted();
			
			/**
			 * \brief	Check if the last query could be performed successful
			 * 
			 * Only valid if isQueryCompleted() returns \c true.
			 * 
			 * \return	\c true if the query was successful. Use getResponse() to
			 * 			access the result.
			 */
			static bool
			isSuccess();
			
			/**
			 * \brief	Check error code
			 * 
			 * Only valid if isQueryCompleted() returns \c true while
			 * isSuccess() returns \c false.
			 * 
			 * \return	Error code
			 * \see		xpcc::sab::Error
			 */
			static uint8_t
			getErrorCode();
			
			
			template <typename T>
			static inline const T *
			getResponse();
			
			static inline const void *
			getResponse();
			
			
			static void
			update();
			
		protected:
			enum QueryStatus
			{
				IN_PROGRESS,			///< Query in progress
				SUCCESS,				///< Response sucessfully received
				ERROR_RESPONSE = 0x40,	///< Error in the received message
				ERROR_TIMEOUT = 0x41,	///< No message received within the timeout window
				ERROR_PAYLOAD = 0x42,	///< Wrong payload size
			};
			
			static Interface interface;
			
			static QueryStatus queryStatus;
			static uint8_t expectedResponseLength;
			static xpcc::Timeout<> timer;
			
			static const uint8_t timeout = 10;		///< timeout value in milliseconds
		};
	}
}

#include "master_impl.hpp"

#endif	// XPCC_SAB__MASTER_HPP
