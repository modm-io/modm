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
#define MODM_DW3110_PHY_HPP

#include <array>
#include <cstdint>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/processing/resumable.hpp>
#include <span>

#include "dw3110_definitions.hpp"

namespace modm
{

/**
 * Hardware abstraction layer for DW3110
 *
 * @ingroup  modm_driver_dw3110
 * @author   Elias H.
 * @author   Raphael Lehmann
 */
template<typename SpiMaster, typename Cs, uint32_t buffer_size>
class Dw3110Phy : public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<3>
{

public:
	/**
	 */
	modm::ResumableResult<void>
	initialize();

	modm::ResumableResult<bool>
	ping();

	modm::ResumableResult<bool>
	transmit(std::span<uint8_t> payload);

	modm::ResumableResult<bool>
	receive(void);
	uint8_t receive_rx_data[20];
	std::span<uint8_t> receive_rx{receive_rx_data};
	uint8_t receive_state_data[4];
	std::span<uint8_t> receive_state{receive_state_data};

private:
	std::array<uint8_t, buffer_size> tx_buffer;
	std::array<uint8_t, buffer_size> rx_buffer;

	uint8_t ping_result_data[4];
	std::span<uint8_t> ping_result{ping_result_data};

	modm::ResumableResult<uint32_t>
	send(modm::Dw3110Register::FastCommand cmd);

	template<modm::Dw3110Register::SpiMode mode>
	modm::ResumableResult<uint32_t>
	send(modm::Dw3110Register::DwRegister address);

	template<modm::Dw3110Register::SpiMode mode>
	modm::ResumableResult<void>
	send(modm::Dw3110Register::DwRegister address, std::span<uint8_t> data);

	template<modm::Dw3110Register::SpiMode mode>
	modm::ResumableResult<void>
	send(uint16_t address, std::span<uint8_t> data);
	uint16_t send_tx_header = 0;
	uint32_t send_size = 0;

	uint8_t tmp_data[4] = {0,0,0b11000,0b1100};//{0b1100,0b11000,0,0};
	std::span<uint8_t> tmp{tmp_data};
};

}  // namespace modm

#include "dw3110_phy_impl.hpp"

#endif
