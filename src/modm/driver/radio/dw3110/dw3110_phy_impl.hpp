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

template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::initialize()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(nullptr, nullptr, 1));

	if (this->releaseMaster()) { Cs::set(); }

	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::ping()
{
	uint16_t tx_header = 0;
	uint32_t rx_data = 0;
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	tx_header = static_cast<uint16_t>(modm::Dw3110Register::SpiMode::FULL_READ) << 8;
	tx_header |= static_cast<uint16_t>(modm::Dw3110Register::DwRegister::DEV_ID);
	*((uint16_t *)tx_buffer.data()) = modm::toBigEndian(tx_header);

	RF_CALL(SpiMaster::transfer(tx_buffer.data(), rx_buffer.data(), 2 + 4));

	// To read the response ignore the first 2 Byte then store the response 4 Byte
	rx_data = *((uint32_t *)((uint8_t *)rx_buffer.data() + 2));

	if (this->releaseMaster()) { Cs::set(); }

	// Ignore minor changes wich are tracked in the lower 4 bits
	RF_END_RETURN((rx_data & ~(0b1111)) == 0xDECA0300);
}
