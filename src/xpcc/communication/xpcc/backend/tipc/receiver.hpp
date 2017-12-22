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

#ifndef XPCC_TIPC__RECEIVER_HPP
#define XPCC_TIPC__RECEIVER_HPP

#include <queue>

#include <mutex>
#include <thread>
#include <memory>

#include <xpcc/container/smart_pointer.hpp>

#include "receiver_socket.hpp"

namespace xpcc
{
	namespace tipc
	{
		/**
		 * \brief	Receive Packets over the TIPC and store them.
		 *
		 * In a separate thread the packets are taken from the TIPC and saved local.
		 *
		 * \ingroup	tipc
		 * \author	Carsten Schmitt
		 */
		class Receiver
		{
		public:
			/**
			 * \param ignoreTipcPortId from this port all messages will be ignored, use this to ignore own transmitted messanges
			 *
			 * \see TransmitterSocket::getPortId
			 */
			Receiver(uint32_t ignoreTipcPortId);

			~Receiver();

			void
			setDomainId(unsigned int id);

			void
			addEventId(uint8_t id);

			void
			addReceiverId(uint8_t id);

			/// Check if a new packet has arrived
			bool
			hasPacket() const;

			/**
			 * \brief	Read the packet
			 *
			 * This is only valid if hasPacket() has returned \c true.
			 */
			const xpcc::SmartPointer&
			getPacket() const;

			/**
			 * \brief	Drop the current Packet
			 *
			 * This is only valid if hasPacket() has returned \c true.
			 */
			void
			dropPacket();

		private:
			typedef xpcc::SmartPointer		Payload;
			typedef std::mutex				Mutex;
			typedef std::lock_guard<Mutex>	MutexGuard;
			typedef	std::thread				Thread;

			bool
			isAlive();

			void*
			runReceiver();

			void
			update();

			ReceiverSocket tipcReceiverSocket_;
			uint32_t ignoreTipcPortId_;	// the tipc port ID from that all messages will be ignored
			unsigned int domainId_;

			std::queue<Payload>	packetQueue_;

			std::unique_ptr<Thread> receiverThread_;
			mutable Mutex receiverSocketLock_;
			mutable Mutex packetQueueLock_;

			bool isAlive_;

		};
	}
}

#endif // XPCC_TIPC__RECEIVER_HPP
