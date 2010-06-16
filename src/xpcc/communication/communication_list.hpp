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

#ifndef	XPCC__COMMUNICATION_LIST_HPP
#define	XPCC__COMMUNICATION_LIST_HPP

#include <xpcc/workflow/timeout.hpp>

#include "backend/backend_interface.hpp"
#include "postman/postman.hpp"
#include "callback.hpp"

namespace xpcc
{
	// forward declaration
	class Communication;
	class Callback;
	
	/**
	 * \brief 		Holds information about the response, repeat sending acknowleging for communication.
	 */
	namespace communicationList
	{
		/**
		 * \ingroup 	communication
		 * \brief 		CommunicationListEntry holds information about one Message has to be sent. Is superclass of all entries
		 */
		class Entry
		{
		public:
		
			typedef enum {
				DEFAULT,
				CALLBACK,
			} TypeInfo;
			
			/**
			 * \brief 		Creates one Entry with given header. this->next is initially set Null
			 *
			 * Creates one Entry with given header. this->next is initially set Null.
			 * The const member this->typeInfo is set to typeInfo and never else changed.
			 * this->typeInfo replaces runtime information needed by handling of messages.
			 *
			 * \see CommunicationList
			 */
			Entry(uint8_t typeInfo, const Header& header, SmartPointer& payload);
			Entry(uint8_t typeInfo, const Header& header);
			
			/**
			 * \brief 		Checks if one Response or ack fitts to the Message represented by this Entry.
			 */
			bool
			headerFits(const Header& header) const;
			
			/**
			 * \brief 		Listhandling, return pointer to the next entry.
			 */
			const Entry *
			getNext() const;
			
			/**
			 * \brief 		Communication info, state of sending and retrieving messages and acks.
			 */
			typedef enum {
				WANT_TO_BE_SENT,
				WAIT_FOR_ACK,
				WAIT_FOR_RESPONSE,
			} State;
			
			const uint8_t typeInfo;
			
			/**
			 * \brief 		Listhandling, holds pointer to the next entry.
			 */
			Entry *next;
			
			// todo timeout
			Header header;
			SmartPointer payload;
			State state;
			Timeout<> time;
			uint8_t tries;
		};
		
		class EntryDefault : public Entry
		{
		public:
			EntryDefault(const Header& header);
			EntryDefault(const Header& header, SmartPointer& payload);
		};

		class EntryWithCallback : public Entry
		{
		public:
			EntryWithCallback(const Header& header, SmartPointer& payload, Callback& callback);
			EntryWithCallback(const Header& header, Callback& callback);
			
			Callback responseCallback;
		};

		/**
		 * \ingroup 	communication
		 * \brief 		List containts Entries.
		 *
		 * Entries are lineary linked.
		 * The member this->first points at any time to the dummyFirst.
		 * The member this->last points at any time to the last entry of the list, or to
		 * the dummyEntry if no entries in the list.
		 * this->last->next == Null is at anytime true.
		 *
		 */
		class List
		{
		public:
			/**
			 * \brief 		Creates one initially valid list.
			 * 
			 * \code
			 * this->first = &dummyFirst;
			 * this->last = &dummyFirst;
			 * \endcode
			 */
			List(xpcc::Communication *communication);

			void
			addEvent(const Header& header);
			
			void
			addEvent(const Header& header, SmartPointer& smartPayload);

			void
			addResponse(const Header& header);

			void
			addResponse(const Header& header, SmartPointer& smartPayload);
			
			void
			addActionCall(const Header& header);
			
			void
			addActionCall(const Header& header, SmartPointer& smartPayload);
			
			void
			addActionCall(const Header& header, SmartPointer& smartPayload, Callback& responseCallback);
			
			void
			addActionCall(const Header& header, Callback& responseCallback);
			
			
			/**
			 * \brief 		Does not handle requests which are not acknowlege.
			 *
			 */
			void
			handlePacket(const Header& header, const SmartPointer& payload);
			
			/**
			 * \brief 		Sending messages which are waiting in the list.
			 *
			 */
			void
			handleWaitingMessages(Postman &postman, BackendInterface &backend);
			
		private:
		
			/**
			 * \brief 		Appends one ore more entries to this->last. this->last is updated to the tail of next.
			 *
			 */
			void
			append(Entry *next);
			
			/**
			 * \brief 		fix->next will be set to next. the old entry of fix->next will be set
			 *				to the tail of next. this->last will be updated to tail of next if 
			 *				fix->next==Null, which is equivalent to fix==last.
			 *
			 * \param fix has to be one element already appended to the list.
			 * \param next must not be contained by any list. Any element of the next->next chain must not be contained by any list.
			 */
			void
			insertAfter(Entry *fix, Entry *next);

			/**
			 * \brief 		if (fix->next) then fix->next will be set to fix->next->next. this->last
			 *				will be set to fix if fix->next was the last element.
			 *				The removed element is returned
			 *				if fix was the last element nothing is done and Null is returned.
			 *				For safety the member next of the returned element is set to Null.
			 *
			 * \param fix has to be one element already appended to the list.
			 */
			Entry *
			removeNext(Entry *fix);
			

			/**
			 * \brief 		Handles the callbacks to responses comming from Backend, if one Entry
			 *				is found in the list. Deletes entries and calls the callback function.
			 *
			 * \param backend pointer to the BackendInterface in order to retrieve response information.
			 * \param e entry in the list. e->next has to fit to the packet actually hold in backend.
			 *
			 */
			template<typename C>
			void
			callOfNextOfEWithCallback(const BackendInterface &backend, Entry *e);

			xpcc::Communication *communication;
			EntryDefault dummyFirst;
			Entry *first;
			Entry *last;
		};
	}
}
// the forwarddeclaration of Communication must be defined here
#include "communication.hpp"

#endif // XPCC__COMMUNICATION_LIST_HPP
