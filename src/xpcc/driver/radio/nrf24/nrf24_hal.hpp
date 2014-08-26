// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__NRF24_HPP
#define XPCC__NRF24_HPP

#include <stdint.h>

namespace xpcc
{

namespace nrf24
{

/* Raw bits */
constexpr uint8_t Bit0 = (1 << 0);
constexpr uint8_t Bit1 = (1 << 1);
constexpr uint8_t Bit2 = (1 << 2);
constexpr uint8_t Bit3 = (1 << 3);
constexpr uint8_t Bit4 = (1 << 4);
constexpr uint8_t Bit5 = (1 << 5);
constexpr uint8_t Bit6 = (1 << 6);
constexpr uint8_t Bit7 = (1 << 7);


enum Register : uint8_t
{

	CONFIG = 0x00,          ///< Configuration Register
	EN_AA = 0x01,           ///< Enable 'Auto Acknowledgment'
	EN_RX_ADDR = 0x02,	    ///< Enabled RX Addresses
	SETUP_AW = 0x03,	    ///< Setup of Address Widths
	SETUP_RETR = 0x04,	    ///< Setup of Automatic Retransmission
	RF_CH = 0x05,           ///< RF Channel
	RF_SETUP = 0x06,        ///< RF Setup Register
	STATUS = 0x07,          ///< Status Register
	OBSERVE_TX = 0x08,      ///< Transmit observe register
	RPD = 0x09,             ///< Received Power Detector
	RX_ADDR_P0 = 0x0a,      ///< Receive address data pipe 0
	RX_ADDR_P1 = 0x0b,      ///< Receive address data pipe 1
	RX_ADDR_P2 = 0x0c,      ///< Receive address data pipe 2
	RX_ADDR_P3 = 0x0d,      ///< Receive address data pipe 3
	RX_ADDR_P4 = 0x0e,      ///< Receive address data pipe 4
	RX_ADDR_P5 = 0x0f,      ///< Receive address data pipe 5
	TX_ADDR = 0x10,         ///< Transmit address
	RX_PW_P0 = 0x11,        ///< Number of bytes in RX payload in pipe 0
	RX_PW_P1 = 0x12,        ///< Number of bytes in RX payload in pipe 1
	RX_PW_P2 = 0x13,        ///< Number of bytes in RX payload in pipe 2
	RX_PW_P3 = 0x14,        ///< Number of bytes in RX payload in pipe 3
	RX_PW_P4 = 0x15,        ///< Number of bytes in RX payload in pipe 4
	RX_PW_P5 = 0x16,        ///< Number of bytes in RX payload in pipe 5
	FIFO_STATUS = 0x17,     ///< FIFO Status Register
	DYNPD = 0x1c,           ///< Enable dynamic payload length
	FEATURE = 0x1d          ///< Feature Register
};

/* CONFIG register */
enum class Config : uint8_t
{
	PRIM_RX      = Bit0,
	PWR_UP       = Bit1,
	CRC0         = Bit2,
	EN_CRC       = Bit3,
	MASK_MAX_RT  = Bit4,
	MASK_TX_DS   = Bit5,
	MASK_RX_DR   = Bit6
};

/* EN_AA register */
enum class EnAA : uint8_t
{
	ENAA_P0 = Bit0,
	ENAA_P1 = Bit1,
	ENAA_P2 = Bit2,
	ENAA_P3 = Bit3,
	ENAA_P4 = Bit4,
	ENAA_P5 = Bit5
};

/* EN_RXADDR register */
enum class EnRxAddr : uint8_t
{
	ERX_P0 = Bit0,
	ERX_P1 = Bit1,
	ERX_P2 = Bit2,
	ERX_P3 = Bit3,
	ERX_P4 = Bit4,
	ERX_P5 = Bit5
};

/* SETUP_AW register */
enum class SetupAw : uint8_t
{
	AW = Bit0 | Bit1
};

/* SETUP_RETR register */
enum class SetupRetr : uint8_t
{
	ARC = Bit0 | Bit1 | Bit2 | Bit3,
	ARD = Bit4 | Bit5 | Bit6 | Bit7
};

/* RF_CH register */
enum class RfCh : uint8_t
{
	RF_CH = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5 | Bit6
};

/* RF_SETUP register */
enum class RfSetup : uint8_t
{
	RF_PWR       = Bit1 | Bit2,
	RF_DR_HIGH   = Bit3,
	PLL_LOCK     = Bit4,
	RF_DR_LOW    = Bit5,
	CONT_WAVE    = Bit7
};

/* STATUS register */
enum class Status : uint8_t
{
	TX_FULL  = Bit0,
	RX_P_NO  = Bit1 | Bit2 | Bit3,
	MAX_RT   = Bit4,
	TX_DS    = Bit5,
	RX_DR    = Bit6
};

/* OBSERVE_TX register */
enum class ObserveTx : uint8_t
{
	ARC_CNT  = Bit0 | Bit1 | Bit2 | Bit3,
	PLOS_CNT = Bit4 | Bit5 | Bit6 | Bit7
};

/* RPD register*/
enum class Rpd : uint8_t
{
	RPD = Bit0
};

/* RX_PW_P0 register */
enum class RxPwP0 : uint8_t
{
	RX_PW_P0 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
};

/* RX_PW_P1 register */
enum class RxPwP1 : uint8_t
{
	RX_PW_P1 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
};

/* RX_PW_P2 register */
enum class RxPwP2 : uint8_t
{
	RX_PW_P2 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
};

/* RX_PW_P3 register */
enum class RxPwP3 : uint8_t
{
	RX_PW_P3 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
};

/* RX_PW_P4 register */
enum class RxPwP4 : uint8_t
{
	RX_PW_P4 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
};

/* RX_PW_P5 register */
enum class RxPwP5 : uint8_t
{
	RX_PW_P5 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
};

/* FIFO_STATUS register */
enum class FifoStatus : uint8_t
{
	RX_EMPTY = Bit0,
	RX_FULL  = Bit1,
	TX_EMPTY = Bit4,
	TX_FULL  = Bit5,
	TX_REUSE = Bit6
};

/* DYNPD register*/
enum class DynPd : uint8_t
{
	DPL_P0 = Bit0,
	DPL_P1 = Bit1,
	DPL_P2 = Bit2,
	DPL_P3 = Bit3,
	DPL_P4 = Bit4,
	DPL_P5 = Bit5
};

/* FEATURE register */
enum class Feature : uint8_t
{
	EN_DYN_ACK   = Bit0,
	EN_ACK_PAY   = Bit1,
	EN_DPL       = Bit2
};

enum Command : uint8_t
{
	R_REGISTER 			= 0b00000000,	///< 1 -  5 data bytes | lower 5 bit = Register address
	W_REGISTER			= 0b00100000,	///< 1 -  5 data bytes | lower 5 bit = Register address
	R_RX_PAYLOAD 		= 0b01100001,	///< 1 - 32 data bytes
	W_TX_PAYLOAD 		= 0b10100000,	///< 1 - 32 data bytes
	FLUSH_TX 			= 0b11100001,	///<      0 data bytes
	FLUSH_RX 			= 0b11100010,	///<      0 data bytes
	REUSE_TX_PL 		= 0b11100011,	///<      0 data bytes
	R_RX_PL_WID 		= 0b01100000,	///<      1 data bytes
	W_ACK_PAYLOAD 		= 0b10101000,	///< 1 - 32 data bytes | lower 3 bit = Pipe # (000 to 101)
	W_TX_PAYLOAD_NOACK 	= 0b10110000,	///< 1 - 32 data bytes
	NOP 				= 0b11111111	///<      0 data bytes
};
}

/**
* @brief Hardware abstraction layer for nRF24L01+
*
*
* @ingroup	radio
* @author	Daniel Krebs
*/

template <typename Spi, typename Csn>
class Nrf24Hal
{

public:

	/** @brief Initialize nRF24-HAL
	 *
	 * Call this function before using this class!
	 *
	 * @param payload_length	Set fixed payload length in bytes , set 0 for dynamic payload length
	 */
	static void
	initialize(uint8_t payload_length = max_payload_length)
	{
		// When payload length is configured to 0, length will be polled every
		// time a packet is fetched from Rx Fifo
		payload_len = payload_length;
	}

	/** @brief Get the maximum payload size the hardware can transmit in one packet
	 */
	static uint16_t
	getMaxPayload()
	{
		return max_payload_length;
	}


	/** @brief Read simple 8 bit register
	 */
	static uint8_t
	readRegister(nrf24::Register reg);


	/** @brief Write simple 8 bit register
	 */
	static void
	writeRegister(nrf24::Register, uint8_t data);


	/** @brief Read received payload
	 *
	 * Used in RX mode.
	 * Payload is deleted from FIFO after it is read.
	 *
	 *	@param buffer 	buffer where to put payload, should be be 32 byte wide
	 *	@return 		length of received payload
	 */
	static uint8_t
	readRxPayload(uint8_t* buffer);


	/** @brief Write payload to be send
	 *
	 * 	@param buffer	Buffer from where to read the payload
	 * 	@param len		How many bytes the payload is wide
	 */
	static void
	writeTxPayload(uint8_t* buffer, uint8_t len);


	/** @brief Same as writeTxPayload() but disable auto ACK for this packet
	 */
	static void
	writeTxPayloadNoAck(uint8_t* buffer, uint8_t len);


	/** @brief Write payload to be transmitted together with ACK packet
	 *
	 * Used in RX mode.
	 * Maximum three ACK packet payloads can be pending. Payloads with
	 * same Pipe are handled using first in - first out principle. Write
	 * payload: 1– 32 bytes. A write operation always starts at byte 0.
	 *
	 * @param pipe Pipe for which the ACK payload is destined, must be in range 0 to 5
	 * @param buffer Buffer from where to read the payload
	 * @param len How many bytes the payload is wide
	 */
	static void
	writeAckPayload(uint8_t pipe, uint8_t* buffer, uint8_t len);


	/**@brief Flush Tx Fifo
	 *
	 * Used in Tx mode
	 */
	static void
	flushTxFifo();


	/** @brief Flush Rx Fifo
	 *
	 * Used in Rx mode.
	 * Should not be executed during transmission of
	 * acknowledge, that is, acknowledge package will
	 * not be completed.
	 */
	static void
	flushRxFifo();


	/** @brief Reuse last transmitted payload
	 *
	 * Used in PTX mode.
	 * TX payload reuse is active until W_TX_PAYLOAD or FLUSH TX is
	 * executed. TX payload reuse must not be activated or deactivated
	 * during package transmission.
	 */
	static void
	reuseTxPayload();


	/** @brief Read Rx payload width for top of Rx Fifo
	 *
	 * Note: Flush RX FIFO if the read value is larger than 32 bytes.
	 */
	static uint8_t
	readRxPayloadWidth();


	/** @brief Read new status
	 *
	 * Note: status will be automatically updated every time a command is
	 *       issued, so it might not be necessary to call this explicitly.
	 *
	 *  @return Status register
	 */
	static uint8_t
	readStatus();


	/** @brief Set Rx address for a pipe
	 *
	 *	Note: pipe 0 and pipe 1 have a 5 byte wide address while pipes 2 to 5
	 *	      share the upper 4 bytes with pipe 1, so when setting the address
	 *	      of a pipes 2 to 5, only 1 byte (LSB) will be written to SPI.
	 *
	 *	@param pipe		Pipe number, must be in range 0 to 5
	 *	@param address	Address for which packets will be put into pipe, see
	 *					description concerning differences by pipe
	 */
	static void
	setRxAddress(uint8_t pipe, uint64_t address);


	/** @brief Set Tx address
	 *
	 * Used in PTX mode only.
	 * Set RX_ADDR_P0 equal to this address to handle
	 * automatic acknowledge if this is a PTX device with
	 * Enhanced ShockBurst enabled.
	 *
	 * @param address	Address where to send packets
	 */
	static void
	setTxAddress(uint64_t address);


	/** @brief Get Rx address of pipe
	 *
	 *	@param pipe		Pipe number
	 *	@return			Address set for pipe
	 */
	static uint64_t
	getRxAddress(uint8_t pipe);


	/** @brief Get Tx address
	 */
	static uint64_t
	getTxAddress();


	/** @brief
	 *
	 */
	static uint16_t
	getPayloadLength();


private:
	static uint8_t
	writeCommandSingleData(nrf24::Command cmd, uint8_t data);

	static void
	writeCommandNoData(nrf24::Command cmd);

	/** @brief Read and write multiple bytes via SPI
	 *
	 * Supplying NULL as argv or retv is allowed.
	 *  argv = nullptr 	-> 0x00 is send
	 *  retv = nullptr  -> return values are discarded
	 */
	static void
	writeCommandMultiData(nrf24::Command cmd, uint8_t* argv, uint8_t* retv, uint8_t argc);

	static nrf24::Command
	toCommand(nrf24::Command cmd, nrf24::Register reg, uint8_t offset)
	{
		return static_cast<nrf24::Command>((cmd | reg) + offset);
	}

	static nrf24::Command
	toCommand(nrf24::Command cmd, uint8_t offset)
	{
		return static_cast<nrf24::Command>(cmd + offset);
	}

	static nrf24::Register
	toRegister(nrf24::Register reg, uint8_t offset)
	{
		return static_cast<nrf24::Register>(reg + offset);
	}


private:
	static constexpr uint8_t max_payload_length = 23;
	static constexpr uint8_t address_size = 5;			/** Size of Rx/Tx addresses in bytes */
	static constexpr uint8_t rx_pipe_count = 6;			/** Number of Rx pipes (0 to 5) */
	static uint8_t status;
	static uint8_t payload_len;							/** Fixed length of payload in bytes. 0 means dynamic payload*/

};

}

#include "nrf24_hal_impl.hpp"

#endif
