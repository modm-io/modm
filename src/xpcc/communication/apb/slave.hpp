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
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_APB__SLAVE_HPP
#define	XPCC_APB__SLAVE_HPP

#include <xpcc/architecture/general/accessor/flash.hpp>

#include "interface.hpp"

namespace xpcc
{
	namespace apb
	{
		/**
		 * \brief	APB Slave
		 * 
		 * \todo	documentation
		 * 
		 * \ingroup	apb
		 * \author	Fabian Greif
		 */
		template <typename INTERFACE>
		class Slave
		{
		public:
			typedef typename INTERFACE::ErrorCode ErrorCode;
			
			struct Action
			{
				typedef void (*Callback)(Slave<INTERFACE>& slave, const uint8_t *payload);
				
				uint8_t command;
				uint8_t payloadLength;
				Callback function;
			};
			
		public:
			/**
			 * \brief	Initialize the slave
			 * 
			 * \param	address		Own address
			 * \param	actionList	List of all action callbacks, need to be
			 * 						stored in flash-memory
			 * \param	actionCount	Number of entries in \a actionList
			 */
			Slave(uint8_t address, xpcc::accessor::Flash<Action> actionList, uint8_t actionCount);
			
			void
			sendErrorResponse(uint8_t errorCode = INTERFACE::GENERAL_ERROR);
			
			void
			sendResponse();
			
			void
			sendResponse(const uint8_t *payload, uint8_t payloadLength);
			
			void
			update();
			
		private:
			uint8_t ownAddress;
			xpcc::accessor::Flash<Action> actionList;
			uint8_t actionCount;
			
			uint8_t currentCommand;
		};
	}
}

#include "slave_impl.hpp"

#endif	// XPCC_APB__SLAVE_HPP
