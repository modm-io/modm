/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_CAN_CONNECTOR_HPP
#define	XPCC_CAN_CONNECTOR_HPP

#include <modm/container/linked_list.hpp>
#include "../backend_interface.hpp"

// Filter
#define XPCC_CAN_PACKET_DESTINATION(x)		(static_cast<uint32_t>(x) << 16)
#define XPCC_CAN_PACKET_SOURCE(x)			(static_cast<uint32_t>(x) << 8)
#define XPCC_CAN_PACKET_ID(x)				(static_cast<uint32_t>(x) << 0)

#define XPCC_CAN_PACKET_TYPE_REQUEST		(0x00UL << 24)
#define XPCC_CAN_PACKET_ACKNOWLEDGE			(0x04UL << 24)

#define	XPCC_CAN_PACKET_EVENT				(XPCC_CAN_PACKET_TYPE_REQUEST | XPCC_CAN_PACKET_DESTINATION(0))

// Filter Mask
#define XPCC_CAN_PACKET_DESTINATION_MASK	XPCC_CAN_PACKET_DESTINATION(0xff)
#define XPCC_CAN_PACKET_SOURCE_MASK			XPCC_CAN_PACKET_SOURCE(0xff)
#define XPCC_CAN_PACKET_ID_MASK				XPCC_CAN_PACKET_ID(0xff)

#define XPCC_CAN_PACKET_TYPE_MASK			(0x18UL << 24)
#define XPCC_CAN_PACKET_ACKNOWLEDGE_MASK	(0x04UL << 24)

#define	XPCC_CAN_PACKET_EVENT_MASK			(XPCC_CAN_PACKET_DESTINATION_MASK | XPCC_CAN_PACKET_TYPE_MASK)

namespace xpcc
{
	class CanConnectorBase
	{
	public:
		/// Convert a packet header to a can identifier
		static uint32_t
		convertToIdentifier(const Header & header, bool fragmentated);

		/**
		 * \brief	Convert a can identifier to a packet header
		 *
		 * \param[in]	identifier	29-bit CAN identifier
		 * \param[out]	header		Packet header
		 *
		 * \return	\c true if the message is part of a fragmented packet,
		 * 			\c false otherwise.
		 */
		static bool
		convertToHeader(const uint32_t & identifier, Header & header);

		static inline bool
		isFragment(const uint32_t & identifier)
		{
			const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&identifier);
			return (ptr[3] & 0x01);
		}

		/**
		 * \brief	Calculate the number of fragments needed to send a message
		 * 			with a length of \p messageSize.
		 */
		static uint8_t
		getNumberOfFragments(uint8_t messageSize);

	protected:
		static uint8_t messageCounter;
	};

	/**
	 * \brief	The CAN connector to the XPCC communication
	 *
	 * \section can_interface_definition The Needed Interface
	 *
	 * The interface of the per template parameter given driver has to
	 * provide the following static methods.
	 *
	 * \code
	 * static bool
	 * isMessageAvailable();
	 *
	 * static bool
	 * getMessage(can::Message& message);
	 *
	 * /// The CAN controller has a free slot to send a new message.
	 * /// \return true if a slot is available, false otherwise
	 * static bool
	 * isReadyToSend();
	 *
	 * /// Send a message over the CAN.
	 * /// \return true if the message was send, false otherwise
	 * static bool
	 * sendMessage(const can::Message& message);
	 * \endcode
	 *
	 * \section structure Definition of the structure of a CAN message
	 *
	 * \image html xpcc_can_identifier.png
	 *
	 * Changes in the highest 4 bits:
	 * - 2 bit: Action [0], Response [1], Neg. Response [2], not used [3]
	 * - 1 bit: Request [0], Acknowledge [1] (NACK implicit in the payload)
	 * - 1 bit: Message Counter active [1] / not active [1]
	 *
	 * Every event is send with the destination identifier \c 0x00.
	 *
	 * \todo timeout
	 *
	 * \ingroup	modm_communication_xpcc_backend
	 */
	template <typename Driver>
	class CanConnector : protected CanConnectorBase, public BackendInterface
	{
	public:
		CanConnector(Driver *driver);

		virtual
		~CanConnector();

		virtual void
		sendPacket(const Header &header, modm::SmartPointer payload);


		virtual bool
		isPacketAvailable() const;

		virtual const Header&
		getPacketHeader() const;

		virtual const modm::SmartPointer
		getPacketPayload() const;

		virtual void
		dropPacket();


		virtual void
		update();

	protected:
		CanConnector(const CanConnector&);

		CanConnector&
		operator = (const CanConnector&);

		/**
		 * \brief	Try to send a CAN message via CAN Driver
		 *
		 * \return	\b true if the message could be send, \b false otherwise
		 */
		bool
		sendMessage(const uint32_t & identifier,
				const uint8_t *data, uint8_t size);

		void
		sendWaitingMessages();

		bool
		retrieveMessage();

	protected:
		class SendListItem
		{
		public:
			SendListItem(const uint32_t & inIdentifier,
					const modm::SmartPointer& inPayload) :
				identifier(inIdentifier),
				payload(inPayload),
				fragmentIndex(0)
			{
			}

			SendListItem(const SendListItem& other) :
				identifier(other.identifier),
				payload(other.payload),
				fragmentIndex(other.fragmentIndex)
			{
			}

			uint32_t identifier;
			modm::SmartPointer payload;

			uint8_t fragmentIndex;

		private:
			SendListItem&
			operator = (const SendListItem& other);
		};

		class ReceiveListItem
		{
		public:
			ReceiveListItem(uint8_t size, const Header& inHeader,
					uint8_t messageCounter = 0) :
				header(inHeader), payload(size),
				receivedFragments(0),
				counter(messageCounter)
			{
			}

			ReceiveListItem(const ReceiveListItem& other) :
				header(other.header), payload(other.payload),
				receivedFragments(other.receivedFragments),
				counter(other.counter)
			{
			}

			Header header;
			modm::SmartPointer payload;

			uint8_t receivedFragments;
			const uint8_t counter;

		private:
			ReceiveListItem&
			operator = (const ReceiveListItem& other);
		};

		typedef modm::LinkedList< SendListItem > SendList;
		typedef modm::LinkedList< ReceiveListItem > ReceiveList;

	protected:
		SendList sendList;
		ReceiveList pendingMessages;
		ReceiveList receivedMessages;

		Driver *canDriver;
	};
}

#include "connector_impl.hpp"

#endif	// XPCC_CAN_CONNECTOR_HPP
