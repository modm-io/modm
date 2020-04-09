/*
 * Copyright (c) 2014-2015, 2018, Daniel Krebs
 * Copyright (c) 2014, 2016, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 * Copyright (c) 2019, Stefan Voigt
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NRF24_DATA_HPP
#define MODM_NRF24_DATA_HPP

#include <stdint.h>
#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>
#include <modm/architecture/interface/clock.hpp>

#include "nrf24_phy.hpp"
#include "nrf24_config.hpp"
#include "nrf24_definitions.hpp"

namespace modm
{

/* Pipe layout:
 *
 * ===== 0 ===== : Used to receive ACKs when communicating directly with another node
 * ===== 1 ===== : Broadcast pipe, will determine upper 4 bytes of address of pipes 2 - 5
 * ===== 2 ===== : Own address
 * ===== 3 ===== : \
 * ===== 4 ===== :  | Separate connections to neighbouring nodes
 * ===== 5 ===== : /
 *
 * NOTE: connections on pipe 3-5 not yet implemented
 */

/// @ingroup	modm_driver_nrf24
/// @author		Daniel Krebs
template<typename Nrf24Phy, typename Clock = modm::Clock>
class Nrf24Data : modm::Nrf24Register
{
public:
	using BaseAddress = uint64_t;
	using Address     = uint8_t;
	using Timestamp   = modm::Timestamp;
	using ClockLower  = Clock;

	enum class SendingFeedback
	{
		Busy,           ///< Waiting for ACK
		FinishedAck,    ///< Packet sent and ACK received
		FinishedNack,   ///< Packet sent but no ACK received in time
		DontKnow,       ///< Packet was sent without requesting ACK
		Failed,         ///< Packet could not be sent
		Undefined       ///< Initial state before a packet has been handled
	};

	static const char*
	toStr(SendingFeedback feedback) {
		switch(feedback) {
		case SendingFeedback::Busy:			return "Busy";
		case SendingFeedback::FinishedAck:	return "FinishedAck";
		case SendingFeedback::FinishedNack:	return "FinishedNack";
		case SendingFeedback::DontKnow:		return "DontKnow";
		case SendingFeedback::Failed:		return "Failed";
		default:
		case SendingFeedback::Undefined:	return "Undefined"; }
	}

	struct Feedback {
		// TX feedback
		/// only valid for sent packets
		SendingFeedback	sendingFeedback;

		// RX feedback
		/// only valid for received packets
		Timestamp timestamp;
		/// whether the corresponding interrupt has been missed or not
		bool timestampTrusted;
	};

	/// Header of Frame
	struct modm_packed Header
	{
		Address	src;
		Address	dest;
	};

	/// Data that will be sent over the air
	class modm_packed Packet
	{
		friend Nrf24Data;

	private:
		Header	header;
	public:
		uint8_t	payload[Nrf24Phy::getMaxPayload() - sizeof (Header)];

	public:
		static constexpr uint8_t
		getPayloadLength()
		{ return sizeof(payload); }

		inline void
		setDestination(Address dest)
		{ header.dest = dest; }

		inline Address
		getDestination() const
		{ return header.dest; }

		inline Address
		getSource() const
		{ return header.src; }

	private:
		inline void
		setSource(Address src)
		{ header.src = src; }
	};
	/// @}

	static constexpr uint8_t
	getFrameOverhead()
	{ return sizeof (Header); }

	static uint8_t
	getDynamicPayloadLength()
	{ return Phy::getPayloadLength() - getFrameOverhead(); }

public:

	/* type alias config and physical layer for simplicity */
	using Config = modm::Nrf24Config<Nrf24Phy>;
	using Phy = Nrf24Phy;

	static void
	initialize(BaseAddress base_address,
	           Address own_address,
	           Address broadcast_address = 0xFF);

public:

	static bool
	sendPacket(Packet& packet);

	static bool
	getPacket(Packet& packet);

	static bool
	isReadyToSend()
	{ return feedbackLastPacket.sendingFeedback != SendingFeedback::Busy; }

	static bool
	isPacketAvailable();

	/// Feedback of last transaction, i.e. calls to `sendPacket()` and
	/// `getPacket()`.
	static const Feedback&
	getFeedback()
	{ return feedbackLastPacket; }

	/// Call this function in your main loop
	static void
	update();

	/// Call this function from within IRQ pin interrupt
	/// You have to setup the IRQ pin yourself!
	static void
	interruptHandler();

	static Address
	getAddress()
	{ return ownAddress; }

	/// Set own address
	static void
	setAddress(Address address);

	static void
	setBroadcastAddress(Address address);

	static Address
	getBroadcastAddress()
	{ return broadcastAddress; }

private:

	static uint64_t
	assembleAddress(Address address) {
		return static_cast<uint64_t>((uint64_t)baseAddress | (uint64_t)address);
	}

	static bool
	updateSendingState();

private:

	/**  Base address of the network
	 *
	 *   The first 3 bytes will be truncated, so the address is actually 5 bytes
	 *   long. The last byte will be used to address individual modules or
	 *   connections between them respectively. Use different base address for
	 *   separate networks, although it may impact performance seriously to run
	 *   overlapping networks.
	 *
	 *   Format:
	 *
	 *   | dont'care | base address | address |
	 *   | ---------------------------------- |
	 *   |   3 byte  |    4 byte    |  1 byte |
	 */
	static inline BaseAddress baseAddress;

	/// Packet with this destination will be sent without acknowledgements and
	/// delivered to all stations in vicinity.
	static inline Address broadcastAddress;

	/// Network address of this station
	static inline Address ownAddress;

	/// Internal state of feedback. Is double-buffered by `feedbackLastPacket`
	static inline Feedback feedbackCurrentPacket;

	/// Feedback for the last sent or received packet (refers to calls to
	/// `getPacket()` and `sendPacket()`).
	static inline Feedback feedbackLastPacket;

	/// Workaround when nrf24 module doesn't indicate finished transmission. The
	/// reason is still unknown, but without this timeout, the internal state
	/// machine of the driver would get stuck.
	static inline modm::Timeout sendingInterruptTimeout;

	/// How long to wait for a finished transmission before aborting (see
	/// `sendingInterruptTimeout` for more details).
	/// Depending on the amount of retransmissions and the auto retransmit delay
	/// this timeout can become quite large. Worst case is 4000us * 15 = 60ms
	/// plus some processing times. We consider the worst case here.
	static constexpr std::chrono::milliseconds sendingInterruptTimeoutMs{65};
};

}	// namespace modm

#include "nrf24_data_impl.hpp"

#endif /* MODM_NRF24_DATA_HPP */
