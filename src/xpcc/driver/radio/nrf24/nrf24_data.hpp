#ifndef NRF24_DATA_HPP
#define NRF24_DATA_HPP

#include "nrf24_phy.hpp"

template<typename Nrf24Phy>
class Nrf24Data
{

public:

	typedef uint64_t 	BaseAddress;
	typedef uint8_t 	Address;

	enum class SendingState
	{
		BUSY,
		FINISHED_ACK,
		FINISHED_NACK,
		DONT_KNOW
	};

	typedef struct packet_t
	{
		Address		src;
		Address 	dest;
		uint8_t*	data;
		uint8_t 	length;
	} packet_t;


public:

	static void
	initialize(BaseAddress base_address, Address broadcast_address = 0xFF)
	{
		baseAddress = base_address;
		broadcastAddress = broadcast_address;

		// ... configure phy
	}


public:

	/* general data layer interface */

	static bool
	sendPacket(packet_t&);

	static bool
	getPacket(packet_t&);

	static bool
	isReadyToSend();

	static bool
	isPacketAvailable();

	static SendingState
	getSendingFeedback();

	static Address
	getAddress();

	static void
	setAddress();

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

	static bool
	establishConnection();

	static bool
	destroyConnection();


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

	static Address connections[4];
};


#endif /* NRF24_DATA_HPP */
