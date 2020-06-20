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

#ifndef MODM_SX1276_HPP
#define MODM_SX1276_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/debug/logger.hpp>

#include "sx1276_definitions.hpp"

namespace modm
{

/**
 * @tparam    SpiMaster    SpiMaster interface
 * @tparam    Cs           Chip-select pin
 *
 * @author    Benjamin Carrick
 * @ingroup   modm_driver_sx1276
 */
template <typename SpiMaster, typename Cs>
class Sx1276 : public sx1276, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<6>
{
public:
	/**
	 * \brief Construct a new Sx1276 Driver
	 */
	Sx1276();

	/**
	 * \brief Initialize the modem into Lora mode
	 */
	modm::ResumableResult<void>
	initialize();

	/**
	 * \brief Setup the modem parameters
	 *
	 * \param bw    The Bandwidth of the LoRa Transmission
	 * \param sf    The Spreading factor for each symbol
	 * \param cr    The Coding rate for the symbols
	 * \param implicitHeader Run the modem in implicit header mode
	 * \param payloadCrc Append CRC checksums to validate the packages
	 */
	modm::ResumableResult<void>
	setModemParams( Bandwidth bw,
					SpreadingFactor sf,
					CodingRate cr,
					bool implicitHeader,
					bool payloadCrc);

	/**
	 * \brief Set the carrier frequency of the Modem
	 *
	 * \param freq The Carrier frequency the modem will be tuned to
	 */
	modm::ResumableResult<void>
	setCarrierFrequency(frequency_t freq);

	/**
	 * \brief Set the sync word for the LoRa packages
	 *
	 * \param syncWord The new sync word of upcoming lora packages
	 */
	modm::ResumableResult<void>
	setSyncWord(uint8_t syncWord);

	/**
	 * \brief Transmit a single package
	 *
	 * \param data A pointer to the payload data of the Package
	 * \param length The length of the payload data in bytes
	 * \return Returns true if transmission has started. False if the buffer was already sending
	 */
	modm::ResumableResult<bool>
	transmit(uint8_t* data, uint8_t length);

	/**
	 * \brief Enable the Rx mode and start listening for packages
	 */
	modm::ResumableResult<void>
	enableListening();

	/**
	 * \brief Disable Rx mode
	 */
	modm::ResumableResult<void>
	disableListening();

	/**
	 * \brief Reads a package from the modem if one is available.
	 * The return value indicates how many bytes have been read from the last received package.
	 * If the return value is 0 no package has been received, is it larger than the max length
	 * parameter, the package was bigger than the buffer provided and got discarded
	 *
	 * \param data A pointer to a buffer where received data will be stored
	 * \param maxLength The maximum amount of bytes that can be read
	 * \return The amount of bytes in the last package
	 */
	modm::ResumableResult<uint8_t>
	readPacket(uint8_t* data, uint8_t maxLength);

	/**
	 * \brief Get the signal to noise ratio for the last received packet
	 *
	 * \return The raw signal to noise ratio in dB (has to be divided by 4 for the actual value)
	 */
	modm::ResumableResult<int8_t>
	getPacketSnr();

	/**
	 * \brief Get the recieved signal strength indicator (RSSI) of the last received packet
	 *
	 * \return The RSSI value of the last received packet
	 */
	modm::ResumableResult<int16_t>
	getPacketRssi();

	/**
	 * \brief Get the current recieved signal strength indicator (RSSI)
	 *
	 * \return The RSSI value of the last received packet
	 */
	modm::ResumableResult<int16_t>
	getCurrentRssi();

private:

	/// The frequency of the connected oscillator
	static constexpr frequency_t osc_freq = 32_MHz;

	/// The internal buffer for spi transfers
	uint8_t buffer[4];

	// Shadow registers of the chip registers

	OpModeRegister_t opModeShadow;
	ModemConfig1_t modemConf1Shadow;
	ModemConfig2_t modemConf2Shadow;
	ModemConfig3_t modemConf3Shadow;
	int32_t frfShadow;
	Interrupts_t irqMaskShadow;

	/// The latest irq flags
	Interrupts_t irqFlags;
	/// Result of the last transmission attempt
	bool lastTransmitResult;
	/// The payload size of the last packet
	uint8_t lastPacketSize;
	/// Temporary storage for RSSI values
	int16_t tempRssi;

	/// Reads a single register
	modm::ResumableResult<uint8_t>
	readRegister(Sx1276Register reg);

	/// Writes a single register with a given value
	modm::ResumableResult<void>
	writeRegister(Sx1276Register reg, uint8_t value);

	/// Transfers the buffer to the SPI bus
	modm::ResumableResult<void>
	transferBuffer(uint8_t length);

	/// change the operation mode of the modem
	modm::ResumableResult<void>
	changeMode(ModemMode mode);

};

}

#include "sx1276_impl.hpp"

#endif
