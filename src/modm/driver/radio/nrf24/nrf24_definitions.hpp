/*
 * Copyright (c) 2014, Niklas Hauser
 * Copyright (c) 2014-2015, Daniel Krebs
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NRF24_DEFINITIONS_HPP
#define MODM_NRF24_DEFINITIONS_HPP

#include <stdint.h>
#include <modm/architecture/interface/register.hpp>
#include <modm/utils/bit_constants.hpp>

namespace modm
{

struct Nrf24Register
{
	/* Commands */

	enum class
	Command : uint8_t
	{
		R_REGISTER          = 0b00000000,   ///< 1 -  5 data bytes | lower 5 bit = Register address
		W_REGISTER          = 0b00100000,   ///< 1 -  5 data bytes | lower 5 bit = Register address
		R_RX_PAYLOAD        = 0b01100001,   ///< 1 - 32 data bytes
		W_TX_PAYLOAD        = 0b10100000,   ///< 1 - 32 data bytes
		FLUSH_TX            = 0b11100001,   ///<      0 data bytes
		FLUSH_RX            = 0b11100010,   ///<      0 data bytes
		REUSE_TX_PL         = 0b11100011,   ///<      0 data bytes
		R_RX_PL_WID         = 0b01100000,   ///<      1 data bytes
		W_ACK_PAYLOAD       = 0b10101000,   ///< 1 - 32 data bytes | lower 3 bit = Pipe # (000 to 101)
		W_TX_PAYLOAD_NOACK  = 0b10110000,   ///< 1 - 32 data bytes
		NOP                 = 0b11111111    ///<      0 data bytes
	};
	MODM_FLAGS8(Command);

	enum class
	Pipe : uint8_t
	{
		PIPE_0 = 0b000,
		PIPE_1 = 0b001,
		PIPE_2 = 0b010,
		PIPE_3 = 0b011,
		PIPE_4 = 0b100,
		PIPE_5 = 0b101,
		//PIPE_6 = 0b110    // TODO: check if this is legal, data sheet says it isn't ...
	};
	MODM_FLAGS8(Pipe);

	enum class
	NrfRegister : uint8_t
	{
		CONFIG = 0x00,          ///< Configuration Register
		EN_AA = 0x01,           ///< Enable 'Auto Acknowledgment'
		EN_RX_ADDR = 0x02,      ///< Enabled RX Addresses
		SETUP_AW = 0x03,        ///< Setup of Address Widths
		SETUP_RETR = 0x04,      ///< Setup of Automatic Retransmission
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
	MODM_FLAGS8(NrfRegister);

	/* Not every element possible is legal, please refer to RawCommand or to
	 * data sheet on page 51. Only certain RawCommands have to be combined with
	 * a register or pipe.
	 */
//	typedef FlagsGroup<
//			RawCommand_t, Register_t, Pipe_t
//	> Command_t;


	/* Register definitions */

	enum class
	Config : uint8_t
	{
		PRIM_RX      = Bit0,
		PWR_UP       = Bit1,
		CRC0         = Bit2,
		EN_CRC       = Bit3,
		MASK_MAX_RT  = Bit4,
		MASK_TX_DS   = Bit5,
		MASK_RX_DR   = Bit6
	};
	MODM_FLAGS8(Config);

	enum class
	EnAA : uint8_t
	{
		ENAA_P0 = Bit0,
		ENAA_P1 = Bit1,
		ENAA_P2 = Bit2,
		ENAA_P3 = Bit3,
		ENAA_P4 = Bit4,
		ENAA_P5 = Bit5
	};
	MODM_FLAGS8(EnAA);

	enum class
	EnRxAddr : uint8_t
	{
		ERX_P0 = Bit0,
		ERX_P1 = Bit1,
		ERX_P2 = Bit2,
		ERX_P3 = Bit3,
		ERX_P4 = Bit4,
		ERX_P5 = Bit5
	};
	MODM_FLAGS8(EnRxAddr);

	enum class
	SetupAw : uint8_t
	{
		AW = Bit0 | Bit1
	};
	MODM_FLAGS8(SetupAw);

	enum class
	SetupRetr : uint8_t
	{
		ARC = Bit0 | Bit1 | Bit2 | Bit3,
		ARD = Bit4 | Bit5 | Bit6 | Bit7
	};
	MODM_FLAGS8(SetupRetr);

	enum class
	RfCh : uint8_t
	{
		RF_CH = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5 | Bit6
	};
	MODM_FLAGS8(RfCh);

	enum class
	RfSetup : uint8_t
	{
		RF_PWR       = Bit1 | Bit2,
		RF_DR_HIGH   = Bit3,
		PLL_LOCK     = Bit4,
		RF_DR_LOW    = Bit5,
		CONT_WAVE    = Bit7
	};
	MODM_FLAGS8(RfSetup);

	enum class
	Status : uint8_t
	{
		TX_FULL  = Bit0,
		RX_P_NO  = Bit1 | Bit2 | Bit3,
		MAX_RT   = Bit4,
		TX_DS    = Bit5,
		RX_DR    = Bit6
	};
	MODM_FLAGS8(Status);

	enum class
	ObserveTx : uint8_t
	{
		ARC_CNT  = Bit0 | Bit1 | Bit2 | Bit3,
		PLOS_CNT = Bit4 | Bit5 | Bit6 | Bit7
	};
	MODM_FLAGS8(ObserveTx);

	enum class
	Rpd : uint8_t
	{
		RPD = Bit0
	};
	MODM_FLAGS8(Rpd);

	enum class
	RxPwP0 : uint8_t
	{
		RX_PW_P0 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
	};
	MODM_FLAGS8(RxPwP0);

	enum class
	RxPwP1 : uint8_t
	{
		RX_PW_P1 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
	};
	MODM_FLAGS8(RxPwP1);

	enum class
	RxPwP2 : uint8_t
	{
		RX_PW_P2 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
	};
	MODM_FLAGS8(RxPwP2);

	enum class
	RxPwP3 : uint8_t
	{
		RX_PW_P3 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
	};
	MODM_FLAGS8(RxPwP3);

	enum class
	RxPwP4 : uint8_t
	{
		RX_PW_P4 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
	};
	MODM_FLAGS8(RxPwP4);

	enum class
	RxPwP5 : uint8_t
	{
		RX_PW_P5 = Bit0 | Bit1 | Bit2 | Bit3 | Bit4 | Bit5
	};
	MODM_FLAGS8(RxPwP5);

	enum class
	FifoStatus : uint8_t
	{
		RX_EMPTY = Bit0,
		RX_FULL  = Bit1,
		TX_EMPTY = Bit4,
		TX_FULL  = Bit5,
		TX_REUSE = Bit6
	};
	MODM_FLAGS8(FifoStatus);

	enum class
	DynPd : uint8_t
	{
		DPL_P0 = Bit0,
		DPL_P1 = Bit1,
		DPL_P2 = Bit2,
		DPL_P3 = Bit3,
		DPL_P4 = Bit4,
		DPL_P5 = Bit5
	};
	MODM_FLAGS8(DynPd);

	enum class
	Feature : uint8_t
	{
		EN_DYN_ACK   = Bit0,
		EN_ACK_PAY   = Bit1,
		EN_DPL       = Bit2
	};
	MODM_FLAGS8(Feature);

	typedef FlagsGroup<
			Feature_t, DynPd_t, FifoStatus_t, Rpd_t, ObserveTx_t, Status_t,
			RfSetup_t, SetupRetr_t, SetupAw_t, EnRxAddr_t, EnAA_t, Config_t
	> Flags_t;

	enum class
	InterruptFlag : uint8_t
	{
		MAX_RT = Bit4,
		TX_DS  = Bit5,
		RX_DR  = Bit6,
		ALL    = Bit4 | Bit5 | Bit6
	};
	MODM_FLAGS8(InterruptFlag);
};

}   // namespace modm

#endif /* MODM_NRF24_DEFINITIONS_HPP */
