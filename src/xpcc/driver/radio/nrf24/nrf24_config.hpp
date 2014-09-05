#ifndef NRF24_CONFIG_HPP
#define NRF24_CONFIG_HPP

#include "nrf24_phy.hpp"

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
	enum class
	Mode
	{
		Rx,
		Tx
	};
	enum class
	Speed
	{
		kBps500,
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
	}
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


public:
	static void
	powerUp();

	static void
	powerDown();

	static void
	setChannel(uint8_t channel);

	static void
	setMode(Mode mode);

	static void
	setSpeed(Speed speed);

	static void
	setCrc(Crc);

	static void
	setAddressWidth(AddressWidth width);

	static void
	setRfPower(RfPower power);

	static void
	setAutoRetransmitDelay(AutoRetransmitDelay delay);

};


#endif /* NRF24_CONFIG_HPP */
