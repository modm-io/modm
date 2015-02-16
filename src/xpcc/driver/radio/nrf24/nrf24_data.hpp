// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_DATA_HPP
#define XPCC__NRF24_DATA_HPP

#include <stdint.h>

#include "nrf24_phy.hpp"
#include "nrf24_config.hpp"
#include "nrf24_definitions.hpp"

namespace xpcc
{

/* Pipe layout:
 *
 * ===== 0 ===== : Used to receive ACKs when communicating directly with another node
 * ===== 1 ===== : Broadcast pipe, will determine upper 4 bytes of address of pipes 2 - 5
 * ===== 2 ===== : Own address
 * ===== 3 ===== : \
 * ===== 4 ===== :  | Separte connections to neighbouring nodes
 * ===== 5 ===== : /
 *
 */

/// @ingroup	nrf24
/// @author		Daniel Krebs
template<typename Nrf24Phy>
class Nrf24Data : xpcc::Nrf24Register
{
public:

	/// @{
	/// @ingroup	nrf24
	typedef uint64_t    BaseAddress;
	typedef uint8_t     Address;
	/// @}

	/// @ingroup	nrf24
	enum class SendingState
	{
		Busy,
		FinishedAck,
		FinishedNack,
		DontKnow,
		Undefined
	};

	/// @{
	/// @ingroup	nrf24
	/// Data structure that user uses to pass data to the data layer
	typedef struct packet_t
	{
		Address     dest;
		Address     src;        // will be ignored when sending
		uint8_t*    data;
		uint8_t     length;     // max. 30!
	} packet_t;


	/// Header of frame_t
	typedef struct header_t
	{
		uint8_t     src;
		uint8_t     dest;
	} header_t;

	/// Data that will be sent over the air
	typedef struct frame_t
	{
		header_t    header;
		uint8_t     data[30];   // max. possible payload size (32 byte) - 2 byte (src + dest)
	} frame_t;
	/// @}

public:

	/* typedef config and pyhsical layer for simplicity */
	typedef xpcc::Nrf24Config<Nrf24Phy> Config;
	typedef Nrf24Phy Phy;


	static void
	initialize(BaseAddress base_address, Address own_address, Address broadcast_address = 0xFF);

public:

	/* general data layer interface */

	static bool
	sendPacket(packet_t& packet);

	static bool
	getPacket(packet_t& packet);

	static bool
	isReadyToSend();

	static bool
	isPacketAvailable();

	/** @brief Returns feedback for the last packet sent
	 *
	 */
	static SendingState
	getSendingFeedback();

	static Address
	getAddress();

	/** @brief Set own address
	 *
	 */
	static void
	setAddress(Address address);

	static uint8_t
	getPayloadLength()
	{
		return Phy::getPayloadLength() - sizeof(header_t);
	}

	static Address
	getBroadcastAddress()
	{
		return broadcastAddress;
	}

	static packet_t*
	allocatePacket(uint8_t payloadLength);

	/* nrf24 specific */

// not yet implemented
private:
	static bool
	establishConnection();

	static bool
	destroyConnection();


private:

	static inline uint64_t
	assembleAddress(Address address);

	static SendingState
	updateSendingState();

private:

	/** @brief Base address of the network
	 *
	 *   The first 3 byte will be truncated, so the address is actually 5 bytes
	 *   long. The last byte will be used to address individual modules or
	 *   connections between them respectively. Use different base address for
	 *   seperate networks, although it may impact performance seriously to run
	 *   overlapping networks.
	 *
	 *   Format:
	 *
	 *   | dont'care | base address | address |
	 *   | ---------------------------------- |
	 *   |   3 byte  |    4 byte    |  1 byte |
	 */
	static BaseAddress baseAddress;

	static Address broadcastAddress;

	static Address ownAddress;

	static Address connections[3];

	static frame_t assemblyFrame;

	static SendingState state;

};

}	// namespace xpcc

#include "nrf24_data_impl.hpp"

#endif /* XPCC__NRF24_DATA_HPP */
