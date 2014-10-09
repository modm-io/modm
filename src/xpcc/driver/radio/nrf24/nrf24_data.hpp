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

#include "nrf24_phy.hpp"
#include "nrf24_config.hpp"

namespace xpcc
{

namespace nrf24
{

typedef uint64_t 	BaseAddress;
typedef uint8_t 	Address;

enum class SendingState
{
	BUSY,
	FINISHED_ACK,
	FINISHED_NACK,
	DONT_KNOW
};

/*
 * Data structure that user uses to pass data to the data layer
 */
typedef struct packet_t
{
	Address     dest;
	Address     src;        // will be ignored when sending
	uint8_t*    data;
	uint8_t     length;     // max. 30!
} packet_t;


/*
 * Data that will be sent over the air
 */
typedef struct frame_t
{
    uint8_t     src;
    uint8_t     dest;
    uint8_t     data[30];   // max. possible payload size (32 byte) - 2 byte (src + dest)
} frame_t;

}

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


template<typename Nrf24Phy>
class Nrf24Data
{
public:

	static void
	initialize(BaseAddress base_address, Address broadcast_address = 0xFF);

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
		return Nrf24Phy::getPayloadLength() - 2;
	}

	static Address
	getBroadcastAddress()
	{
		return broadcastAddress;
	}

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

private:

	/** @brief Base address of the network
	 *
	 *	 The first 3 byte will be truncated, so the address is actually 5 bytes
	 *	 long. The last byte will be used to address individual modules or
	 *	 connections between them respectively. Use different base address for
	 *	 seperate networks, although it may impact performance seriously to run
	 *	 overlapping networks.
	 *
	 *	 Format:
	 *
	 *	 | dont'care | base address | address |
	 *	 | ---------------------------------- |
	 *	 |   3 byte  |    4 byte    |  1 byte |
	 */
	static BaseAddress baseAddress;

	static Address broadcastAddress;

	static Address ownAddress;

	static Address connections[3];

	static frame_t assemblyFrame;
};

}

#include "nrf24_data_impl.hpp"

#endif /* XPCC__NRF24_DATA_HPP */
