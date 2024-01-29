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
#   error "Don't include this file directly, use 'dw3110_phy.hpp' instead!"
#endif

#include "dw3110_phy.hpp"
//#include <modm/debug/logger.hpp>


template<typename SpiMaster, typename Cs>
modm::ResumableResult<void>
modm::Dw3110Phy<SpiMaster, Cs>::initialize()
{
	RF_BEGIN();

	RF_WAIT_UNTIL(this->acquireMaster());
	Cs::reset();

	RF_CALL(SpiMaster::transfer(nullptr, nullptr, 1));

	if (this->releaseMaster()) {
		Cs::set();
	}

	RF_END();
}
