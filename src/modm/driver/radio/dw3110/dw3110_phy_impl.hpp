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
	Cs::setOutput();
	RF_END();
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<bool>
modm::Dw3110Phy<SpiMaster, Cs>::ping()
{
	RF_BEGIN();
	ping_result = RF_CALL(
		send(modm::Dw3110Register::SpiMode::FULL_READ, modm::Dw3110Register::DwRegister::DEV_ID));
	RF_END_RETURN((ping_result & ~(0b1111)) == 0xDECA0300);
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint32_t>
modm::Dw3110Phy<SpiMaster, Cs>::send(modm::Dw3110Register::SpiMode mode,
									 modm::Dw3110Register::DwRegister address)
{
	RF_BEGIN();
	RF_END_RETURN_CALL(send(mode, address, 0));
}

template<typename SpiMaster, typename Cs>
modm::ResumableResult<uint32_t>
modm::Dw3110Phy<SpiMaster, Cs>::send(modm::Dw3110Register::SpiMode mode,
									 modm::Dw3110Register::DwRegister address, uint32_t data)
{


	RF_BEGIN();
	send_tx_header = 0;
	send_rx_data = 0;

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	send_tx_header = static_cast<uint16_t>(mode) << 8;
	send_tx_header |= static_cast<uint16_t>(address);
	*((uint16_t *)tx_buffer.data()) = modm::toBigEndian(send_tx_header);
	*((uint32_t *)((uint16_t *)tx_buffer.data() + 1)) = modm::toBigEndian(data);

		RF_CALL(SpiMaster::transfer(tx_buffer.data(), rx_buffer.data(), 2 + 4));

	if (mode == modm::Dw3110Register::SpiMode::FULL_READ ||
		mode == modm::Dw3110Register::SpiMode::SHORT_READ)
	{
		send_rx_data = *((uint32_t *)((uint8_t *)rx_buffer.data() + 2));
	}

	if (this->releaseMaster()) { Cs::set(); }

	RF_END_RETURN(send_rx_data);
}
