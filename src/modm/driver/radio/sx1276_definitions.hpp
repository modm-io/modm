/*
 * Copyright (c) 2020, Benjamin Carrick
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SX1276_DEFINITIONS_HPP
#define MODM_SX1276_DEFINITIONS_HPP

#include <stdint.h>
#include <modm/architecture/interface/register.hpp>
#include <modm/math/utils/bit_constants.hpp>

namespace modm
{

/// @ingroup modm_driver_sx1276
struct sx1276
{
public:

	///Definitions of all the Registers of the modem in LoRa mode
	enum class
	Sx1276Register : uint8_t
	{
		Fifo = 0x00,                ///< Fifo I/O for the transceiver
		OpMode = 0x01,              ///< Operation Mode Register
		FrMsb  = 0x06,              ///< MSB of the Carrier Frequency Register
		FrMid  = 0x07,              ///< Middle Byte of the Carrier Frequency Register
		FrLsb  = 0x08,              ///< LSB of the Carrier Frequency Register
		PaConfig = 0x09,            ///< Power output control
		PaRamp = 0x0A,              ///< Power ramp steepness
		Ocp = 0x0B,                 ///< Overload current protection
		Lna = 0x0C,                 ///< Low Noise Amplifier Control
		FifoAddrPtr = 0x0D,         ///< Address Pointer in Fifo Buffer
		FifoTxBaseAddr = 0x0E,      ///< Base Write Address for Tx Fifo
		FifoRxBaseAddr = 0x0F,      ///< Base Read Address for Rx Fifo
		FifoRxCurrentAddr = 0x10,   ///< Fifo Address of the last received packet
		IrqFlagsMask =  0x11,       ///< Interrupt Masks
		IrqFlags = 0x12,            ///< Interrupt Flags
		RxNbBytes = 0x13,           ///< Amount of payload bytes in last received message
		RxHeaderCntValueMsb = 0x14, ///< MSB of valid header counter since last transition to rx mode
		RxHeaderCntValueLsb = 0x15, ///< LSB of valid header counter since last transition to rx mode
		RxPacketCntValueMsb = 0x16, ///< MSB of valid packet counter since last transition to rx mode
		RxPacketCntValueLsb = 0x17, ///< LSB of valid packet counter since last transition to rx mode
		ModemStat = 0x18,           ///< Modem status
		PktSnrValue = 0x19,         ///< Estimation of the signal-to-noise ration of the last packet received
		PktRssiValue = 0x1A,        ///< RSSI value of the last packet received
		RssiValue = 0x1B,           ///< Current RSSI value
		HopChannel = 0x1C,          ///< Configuration of the frequency hopping
		ModemConfig1 = 0x1D,        ///< Lora PHY configuration register 1
		ModemConfig2 = 0x1E,        ///< Lora PHY configuration register 2
		SymbTimeoutLsb = 0x1F,      ///< LSB symbol timeout (MSB is in ModemConfig 2 Register)
		PreambleMsb = 0x20,         ///< MSB of the preamble length
		PreambleLsb = 0x21,         ///< LSB of the preamble length
		PayloadLength = 0x22,       ///< Payload length
		MaxPayloadLength = 0x23,    ///< Maximum payload length
		HopPeriod = 0x24,           ///< Frequency Hopping Period
		FifoRxByteAddr = 0x25,      ///< Current Rx Fifo Pointer
		ModemConfig3 = 0x26,        ///< Lora PHY configuration register 3
		PpmCorrection = 0x27,       ///< Data rate offset control
		FeiMsb = 0x28,              ///< MSB of the estimated Frequency Error
		FeiMid = 0x29,              ///< Middle byte of the estimated Frequency error
		FeiLsb = 0x2A,              ///< LSB of the estimated frequency error
		RssiWideband = 0x2C,        ///< Wideband RSSI measurement
		DetectOptimize = 0x31,      ///< Lora Detection Optimize
		InvertIQ = 0x33,            ///< Invert I and Q
		HighBWOptimize1 = 0x36,     ///< High bandwidth optimization register 1
		DetectionThreshold = 0x37,  ///< Lora detection threshold
		SyncWord = 0x39,            ///< Lora syncword (0x34 for LoraWAN)
		HighBWOptimize2 = 0x3A,     ///< High bandwidth optimization 2
		InvertIQ2 = 0x3B            ///< Set to 0x19 for inverted IQ
	};

	/// Operation mode register
	enum class
	OpModeRegister : uint8_t
	{
		LoraMode = Bit7,            ///< Enables the Lora Mode - if zero the FSK mode is active
		AccessSharedReg = Bit6,     ///< Enables shared access to FSK register in Lora mode
		// Bit 5 : reserved
		// Bit 4 : reserved
		LowFrequencyMode = Bit3    ///<Enables the Low Frequency Mode
		//Bit 2 - 0 Mode of the Modem - See ModemMode
	};
	MODM_FLAGS8(OpModeRegister);

	/// Modem operation modes
	enum class
	ModemMode : uint8_t
	{
		SLEEP = 0b000,          ///< Sleep Mode
		STDBY = 0b001,          ///< Standby Mode (default)
		FSTX =  0b010,          ///< Frequency Synthesis Mode for Transmission
		TX = 0b011,             ///< Transmit Mode
		FSRX = 0b100,           ///< Frequency Synthesis Mode for Reception
		RXCONTINOUS = 0b101,    ///< Continous Receive Mode
		RXSINGLE = 0b110,       ///< Single Receive Mode
		CAD =   0b111,          ///< Channel Activity Detection
	};
	typedef modm::Configuration<OpModeRegister_t, ModemMode, Bit0 | Bit1 | Bit2> ModemMode_t;

	/// Available interrupts with their positions in the flag and mask register
	enum class
	Interrupts : uint8_t
	{
		CAD_DETECED         = 1,
		FHSS_CHANGE_CHANNEL = 2,
		CAD_DONE            = 4,
		TX_DONE             = 8,
		VALID_HEADER        = 16,
		PAYLOAD_CRC_ERROR   = 32,
		RX_DONE             = 64,
		RX_TIMEOUT          = 128
	};
	MODM_FLAGS8(Interrupts);

	//Modem Config

	/// Operation mode register
	enum class
	ModemConfig1 : uint8_t
	{
		//Bit 7 - 4: Bandwidth
		//Bit 3 - 1: Error Coding Rate
		ImplicitHeaderModeOn = Bit0    ///< Headers will not transmitted
	};
	MODM_FLAGS8(ModemConfig1);

	/// LoRa Bandwidth Modes
	enum class
	Bandwidth : uint8_t
	{
		BW_0 = 0x00,  ///< 7.8 kHz
		BW_1 = 0x10,  ///< 10.4 kHZ
		BW_2 = 0x20,  ///< 15.6 kHz
		BW_3 = 0x30,  ///< 20.8 kHz
		BW_4 = 0x40,  ///< 31.25 kHz
		BW_5 = 0x50,  ///< 41.7 kHz
		BW_6 = 0x60,  ///< 62.5 kHz
		BW_7 = 0x70,  ///< 125 kHz
		BW_8 = 0x80,  ///< 250 kHz
		BW_9 = 0x90,  ///< 500 kHz
	};
	typedef modm::Configuration<ModemConfig1_t, Bandwidth, Bit7 | Bit6 | Bit5 | Bit4> Bandwidth_t;

	///LoRa Coding Rate
	enum class
	CodingRate : uint8_t
	{
		CR_4_5  = 0b0010,  ///< Error Coding Rate 4/5
		CR_4_6  = 0b0100,  ///< Error Coding Rate 4/6
		CR_4_7  = 0b0110,  ///< Error Coding Rate 4/7
		CR_4_8  = 0b1000,  ///< Error Coding Rate 4/8
	};
	typedef modm::Configuration<ModemConfig1_t, CodingRate, Bit3 | Bit2 | Bit1 >CodingRate_t;

	// Flag definitions for configuration registers
	enum class
	ModemConfig2 : uint8_t
	{
		//Bit 7 - 4: Spreading Factor
		TxContinousMode = Bit3, ///< Continously Transmit Data
		RxPayloadCrc = Bit2     ///< Messages will be transmitted along with a CRC
	};
	MODM_FLAGS8(ModemConfig2);

	enum class
	ModemConfig3 : uint8_t
	{
		//Bit 7 - 4: Unused
		LowDataRateOptimize = Bit3,  ///< Optimize the modem for symbol times larger than 16ms
		AgcAutoOn = Bit2             ///< Turn on the Automatic Gain Control
	};
	MODM_FLAGS8(ModemConfig3);

	///Available Spreading Factors
	enum class
	SpreadingFactor : uint8_t
	{
		SF_6 = 0x60,    ///< 64 chips/symbol
		SF_7 = 0x70,    ///< 128 chips/symbol
		SF_8 = 0x80,    ///< 256 chips/symbol
		SF_9 = 0x90,    ///< 512 chips/symbol
		SF_10 = 0xA0,   ///< 1024 chips/symbol
		SF_11 = 0xB0,   ///< 2048 chips/symbol
		SF_12 = 0xC0,   ///< 4096 chips/symbol

	};
	typedef modm::Configuration<ModemConfig2_t, SpreadingFactor, Bit7 | Bit6 | Bit5 | Bit4 >SpreadingFactor_t;

	static constexpr int16_t rssiOffsetLF = -164;

}; // struct sx1276

}
#endif
