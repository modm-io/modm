// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_CONFIG_HPP
#define XPCC__NRF24_CONFIG_HPP

#include <stdint.h>
#include "nrf24_phy.hpp"


namespace xpcc
{

namespace nrf24
{

enum class
Mode
{
	Rx,
	Tx
};
enum class
Speed
{
	kBps250,
	MBps1,
	MBps2
};

enum class
Crc
{
	NoCrc,
	Crc1Byte,
	Crc2Byte
};

enum class
AddressWidth : uint8_t
{
	/* 2 byte works in hardware but is illegal according to datasheet */
	/* Byte2 = 0x00, */
	Byte3 = 0x01,
	Byte4 = 0x02,
	Byte5 = 0x03
};

enum class
RfPower : uint8_t
{
	Minus18dBm 	= 0x00,
	Minus12dBm 	= 0x01,
	Minus6dBm  	= 0x02,
	dBm0 		= 0x03
};

enum class
AutoRetransmitDelay : uint8_t
{
	us250 	= 0x00,
	us500 	= 0x01,
	us750 	= 0x02,
	us1000 	= 0x03,
	us1250 	= 0x04,
	us1500 	= 0x05,
	us1750 	= 0x06,
	us2000 	= 0x07,
	us2250 	= 0x08,
	us2500 	= 0x09,
	us2750 	= 0x0A,
	us3000 	= 0x0B,
	us3250 	= 0x0C,
	us3500 	= 0x0D,
	us3750 	= 0x0E,
	us4000 	= 0x0F,
};

enum class
AutoRetransmitCount : uint8_t
{
	Disable			= 0x00,
	Retransmit1 	= 0x01,
	Retransmit2 	= 0x02,
	Retransmit3 	= 0x03,
	Retransmit4 	= 0x04,
	Retransmit5		= 0x05,
	Retransmit6 	= 0x06,
	Retransmit7 	= 0x07,
	Retransmit8 	= 0x08,
	Retransmit9 	= 0x09,
	Retransmit10 	= 0x0A,
	Retransmit11 	= 0x0B,
	Retransmit12 	= 0x0C,
	Retransmit13 	= 0x0D,
	Retransmit14 	= 0x0E,
	Retransmit15 	= 0x0F
};

enum class
Pipe : uint8_t
{
	Pipe0 = 0x01,
	Pipe1 = 0x02,
	Pipe2 = 0x04,
	Pipe3 = 0x08,
	Pipe4 = 0x10,
	Pipe5 = 0x20,
};

}

/** @brief Configuration interface for nRF24L01+
 *
 * 	This class allows for configuration of some aspects of the nRF24L01+
 * 	wireless modules. It doesn't implement every aspect, but hopefully
 * 	all the often used ones.
 */
template<typename Nrf24Phy>
class Nrf24Config
{

public:
	static void
	powerUp();

	static void
	powerDown();

	static void
	setChannel(uint8_t channel);

	static void
	setMode(nrf24::Mode mode);

	static void
	setSpeed(nrf24::Speed speed);

	static void
	setCrc(nrf24::Crc crc);

	static void
	setAddressWidth(nrf24::AddressWidth width);

	static void
	setRfPower(nrf24::RfPower power);

	static void
	setAutoRetransmitDelay(nrf24::AutoRetransmitDelay delay);

	static void
	setAutoRetransmitCount(nrf24::AutoRetransmitCount count);

	static void
	enableFeatureNoAck();

	static void
	disableFeatureNoAck();

	/** @brief Enable Rx Pipe and set payload width
	 *
	 *	@param pipe		Pipe Number
	 */
	static void
	enablePipe(nrf24::Pipe pipe, bool enableAutoAck);

	/** @brief Disable Rx Pipe and set payload width
	 *
	 *	@param pipe		Pipe Number
	 */
	static void
	disablePipe(nrf24::Pipe pipe);

};

}

#include "nrf24_config_impl.hpp"

#endif /* XPCC__NRF24_CONFIG_HPP */
