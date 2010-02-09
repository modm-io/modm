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
 * $Id: tipc_receiver.hpp 91 2009-10-17 15:53:04Z thundernail $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TIPC__RECEIVER_HPP
#define XPCC_TIPC__RECEIVER_HPP

#include <queue>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <xpcc/data_structure/smart_pointer.hpp>

#include "tipc_receiver_socket.hpp"

namespace xpcc
{
	namespace tipc
	{
		/**
		 * \brief		Receive Packets over the TIPC and store them.
		 * 
		 * In a separate thread the packets are taken from the TIPC and saved local.
		 *  
		 * \ingroup		tipc
		 * \author		Carsten Schmitt < >
		 */
		class Receiver
		{
		public:
			Receiver();

			~Receiver();

			void
			addEventId(uint8_t id);

			void
			addReceiverId(uint8_t id);

			bool
			hasPacket() const;

			const xpcc::SmartPointer&
			frontPayload() const;
			
			void 
			popFront();
			
		private:
			typedef xpcc::SmartPointerVolatile		Payload;
			typedef boost::mutex					Mutex;
			typedef boost::mutex::scoped_lock		MutexGuard;
			typedef	boost::thread::thread			Thread;
			
			bool 
			isAlive();
	
			void* 
			runReceiver();
			
			void 
			update();
			
			ReceiverSocket						tipcReceiverSocket_;
			std::queue<Payload>					packetQueue_;
			
			boost::scoped_ptr<Thread>			receiverThread_;
			mutable Mutex						receiverSocketLock_;
			mutable Mutex						packetQueueLock_;
			
			bool								isAlive_;
		};
	}
}

#endif // XPCC_TIPC__RECEIVER_HPP
