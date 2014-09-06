#ifndef NRF24_DATA_HPP
#define NRF24_DATA_HPP

#include "nrf24_phy.hpp"

enum class sendingState
{
	BUSY,
	FINISHED_ACK,
	FINISHED_NACK,
	DONT_KNOW
};

typedef struct frame_t
{
	uint8_t src;
	uint8_t dest;
	uint8_t data[30];
} frame_t;

typedef struct packet_t
{
	uint8_t dest;
	uint8_t* data;
	uint8_t length;
};

template<typename Nrf24Phy>
class Nrf24Data
{

public:

	static void
	initialize(uint64_t base_address)
	{
		baseAddress = base_address;
	}

public:

	/* general data layer interface */

	static bool
	sendPacket();

	static bool
	getPacket(packet_t&);

	static bool
	isReadyToSend();

	static bool
	isPacketAvailable();

	static sendingState
	getSendingFeedback();

	static uint64_t
	getAddress();

	static void
	setAddress();

	static uint8_t
	getPayloadLength()
	{
		return Nrf24Phy::getPayloadLength() - 2;
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
	uint64_t baseAddress;


	uint8_t connections[4];
};


#endif /* NRF24_DATA_HPP */
