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

#include <cstdint>
#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{


/**
 * Hardware abstraction layer for DW3110
 *
 * @ingroup  modm_driver_dw3110
 * @author   Elias Harrer
 * @author   Raphael Lehmann
 */
template <typename SpiMaster, typename Cs>
class Dw3110Phy : public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<3>
{

public:

	/**
	 */
	modm::ResumableResult<void>
	initialize();

private:

};

} // namespace modm

#include "dw3110_phy_impl.hpp"

#endif
