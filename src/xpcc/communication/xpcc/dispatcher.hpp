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

#ifndef	XPCC__DISPATCHER_HPP
#define	XPCC__DISPATCHER_HPP

#include <xpcc/processing/timer.hpp>
#include <xpcc/container/linked_list.hpp>

#include "backend/backend_interface.hpp"
#include "postman/postman.hpp"

#include "response_callback.hpp"

namespace xpcc
{
	/**
	 * \brief
	 *
	 * \todo	Documentation
	 *
	 * \author	Georgi Grinshpun
	 * \ingroup	xpcc_comm
	 */
	class Dispatcher
	{
	public:
		static const uint16_t acknowledgeTimeout = 500;
		static const uint16_t responseTimeout = 100;

	public:
		Dispatcher(BackendInterface *backend, Postman* postman);

		void
		update();

	private:
		/// Does not handle requests which are not acknowledge.
		void
		handlePacket(const Header& header, const SmartPointer& payload);

		/// Sends messages which are waiting in the list.
		void
		handleWaitingMessages();

		/**
		 * \brief 	This class holds information about a Message being send.
		 * 			This is the superclass of all entries.
		 */
		class Entry
		{
		public:
			enum class Type
			{
				Default,
				Callback,
			};

			/// Communication info, state of sending and retrieving messages and acks.
			enum State
			{
				TransmissionPending,
				WaitForACK,
				WaitForResponse,
			};

		public:
			/**
			 * \brief 	Creates one Entry with given header.
			 * 			this->next is initially set Null
			 *
			 * Creates one Entry with given header. this->next is initially
			 * set Null. The const member this->typeInfo is set to typeInfo
			 * and never else changed. this->typeInfo replaces runtime
			 * information needed by handling of messages.
			 */
			Entry(Type type, const Header& inHeader, SmartPointer& inPayload) :
				type(type),
				header(inHeader), payload(inPayload)
			{
			}

			Entry(const Header& inHeader, SmartPointer& inPayload) :
				header(inHeader), payload(inPayload)
			{
			}

			Entry(const Header& inHeader) :
				header(inHeader)
			{
			}

			Entry(const Header& inHeader,
					SmartPointer& inPayload, ResponseCallback& callback_) :
				type(Type::Callback),
				header(inHeader), payload(inPayload),
				callback(callback_)
			{
			}

			/**
			 * \brief 	Checks if a Response or Acknowledge fits to the
			 * 			Message represented by this Entry.
			 */
			bool
			headerFits(const Header& header) const;

			inline void
			callbackResponse(const Header& header, const SmartPointer &payload) const
			{
				this->callback.call(header, payload);
			}

			const Type type = Type::Default;
			const Header header;
			const SmartPointer payload;
			State state = State::TransmissionPending;
			ShortTimeout time;
			uint8_t tries = 0;
		private:
			ResponseCallback callback;
		};

		void
		addMessage(const Header& header, SmartPointer& smartPayload);

		void
		addMessage(const Header& header, SmartPointer& smartPayload,
				ResponseCallback& responseCallback);

		void
		addResponse(const Header& header, SmartPointer& smartPayload);

		inline void
		handleActionCall(const Header& header, const SmartPointer& payload);

		void
		sendAcknowledge(const Header& header);

		using EntryList = LinkedList<Entry>;
		using EntryIterator = EntryList::iterator;

		EntryIterator
		sendMessageToInnerComponent(EntryIterator entry);

		BackendInterface * const backend;
		Postman * const postman;

		EntryList entries;

	private:
		friend class Communicator;
	};
}

#endif // XPCC__DISPATCHER_HPP
