/*
 * Copyright (c) 2024, Elias H.
 * Copyright (c) 2024, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_DW3110_PHY_HPP
#error "Don't include this file directly, use 'dw3110_phy.hpp' instead!"
#endif

#include <modm/debug/logger.hpp>
#include <modm/math/utils/endianness.hpp>

#include "dw3110_definitions.hpp"
#include "dw3110_phy.hpp"

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::initialize()
{
	RF_BEGIN();
	Cs::setOutput(true);
	RF_END();
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::ping()
{
	RF_BEGIN();
	RF_CALL_BLOCKING(send<modm::Dw3110Register::SpiMode::FULL_READ>(
		modm::Dw3110Register::DwRegister::DEV_ID, ping_result));
	RF_END_RETURN((*((uint32_t *)ping_result.data()) & ~(0b1111)) == 0xDECA0300);
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
template<modm::Dw3110Register::SpiMode mode>
modm::ResumableResult<uint32_t>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::send(modm::Dw3110Register::DwRegister address)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(send<mode>(address, std::span<uint8_t>()));
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
modm::ResumableResult<uint32_t>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::send(modm::Dw3110Register::FastCommand cmd)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(send<modm::Dw3110Register::SpiMode::SHORT_WRITE>(static_cast<uint16_t>(cmd),
																		std::span<uint8_t>()));
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
template<modm::Dw3110Register::SpiMode mode>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::send(modm::Dw3110Register::DwRegister address,
												  std::span<uint8_t> data)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(send<mode>(static_cast<uint16_t>(address), data));
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
template<modm::Dw3110Register::SpiMode mode>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::send(uint16_t address, std::span<uint8_t> data)
{
	RF_BEGIN();
	// Sizes differ on short or long spi mode
	if constexpr (mode == modm::Dw3110Register::SpiMode::SHORT_READ ||
				  mode == modm::Dw3110Register::SpiMode::SHORT_WRITE)
	{
		send_size = data.size() + 1;
		send_tx_header = static_cast<uint16_t>(mode);
		send_tx_header |= address;
	} else
	{
		send_size = data.size() + 2;
		send_tx_header = static_cast<uint16_t>(mode) << 8;
		send_tx_header |= address;
	}

	RF_WAIT_UNTIL(this->acquireMaster());
	// Depending on the spi mode clear the tx buffer or coppy the payload into it
	if constexpr (mode == modm::Dw3110Register::SpiMode::FULL_READ)
	{
		std::fill(tx_buffer.begin() + 2, tx_buffer.end(), 0);
	} else if constexpr (mode == modm::Dw3110Register::SpiMode::SHORT_READ)
	{
		std::fill(tx_buffer.begin() + 1, tx_buffer.end(), 0);

	} else if constexpr (mode == modm::Dw3110Register::SpiMode::FULL_WRITE)
	{
		std::copy(data.begin(), data.end(), tx_buffer.begin() + 2);
	} else if constexpr (mode == modm::Dw3110Register::SpiMode::SHORT_WRITE)
	{
		std::copy(data.begin(), data.end(), tx_buffer.begin() + 1);
	}

	// Coppy the tx header to the buffer
	if constexpr (mode == modm::Dw3110Register::SpiMode::SHORT_READ ||
				  mode == modm::Dw3110Register::SpiMode::SHORT_WRITE)
	{
		tx_buffer.at(0) = send_tx_header & 0xFF;
	} else
	{
		*((uint16_t *)tx_buffer.data()) = modm::toBigEndian(send_tx_header);
	}

	Cs::reset();

	RF_CALL(SpiMaster::transfer(tx_buffer.data(), rx_buffer.data(), send_size));

	Cs::set();

	if constexpr (mode == modm::Dw3110Register::SpiMode::FULL_READ)
	{
		std::copy(rx_buffer.begin() + 2, rx_buffer.begin() + 2 + data.size(), data.data());
	} else if constexpr (mode == modm::Dw3110Register::SpiMode::SHORT_READ)
	{
		std::copy(rx_buffer.begin() + 1, rx_buffer.begin() + 1 + data.size(), data.data());
	}

	RF_WAIT_UNTIL(this->releaseMaster());
	RF_END();
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::transmit(std::span<uint8_t> payload)
{
	RF_BEGIN();

	MODM_LOG_DEBUG << "TX:  ";
	for (auto it = payload.begin(); it != payload.end(); it++)
		MODM_LOG_DEBUG << modm::hex << *it << " ";
	MODM_LOG_DEBUG << modm::endl;

	RF_CALL_BLOCKING(send<modm::Dw3110Register::SpiMode::FULL_WRITE>(
		modm::Dw3110Register::DwRegister::SYS_STATE, tmp));

	RF_CALL_BLOCKING(send<modm::Dw3110Register::SpiMode::FULL_WRITE>(
		modm::Dw3110Register::DwRegister::TX_BUFFER, payload));
	RF_CALL_BLOCKING(send(modm::Dw3110Register::FastCommand::CMD_TX));

	RF_END_RETURN(true);
}

template<typename SpiMaster, typename Cs, uint32_t buffer_size>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs, buffer_size>::receive(void)
{
	RF_BEGIN();

	RF_CALL_BLOCKING(send(modm::Dw3110Register::FastCommand::CMD_RX));

	RF_CALL_BLOCKING(send<modm::Dw3110Register::SpiMode::FULL_READ>(
		modm::Dw3110Register::DwRegister::SYS_STATE, receive_state));
	MODM_LOG_DEBUG << "State: ";
	for (auto it = receive_state.rbegin(); it != receive_state.rend(); it++)
		MODM_LOG_DEBUG << modm::hex << *it << " ";
	MODM_LOG_DEBUG << modm::endl;

	RF_CALL_BLOCKING(send<modm::Dw3110Register::SpiMode::FULL_READ>(
		modm::Dw3110Register::DwRegister::RX_BUFFER_0, receive_rx));
	MODM_LOG_DEBUG << "RX0: ";
	for (auto it = receive_rx.rbegin(); it != receive_rx.rend(); it++)
		MODM_LOG_DEBUG << modm::hex << *it << " ";
	MODM_LOG_DEBUG << modm::endl;
	RF_CALL_BLOCKING(send<modm::Dw3110Register::SpiMode::FULL_READ>(
		modm::Dw3110Register::DwRegister::RX_BUFFER_1, receive_rx));
	MODM_LOG_DEBUG << "RX1: ";
	for (auto it = receive_rx.rbegin(); it != receive_rx.rend(); it++)
		MODM_LOG_DEBUG << modm::hex << *it << " ";
	MODM_LOG_DEBUG << modm::endl;

	RF_END_RETURN(true);
}
