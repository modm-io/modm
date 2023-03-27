/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/dma/dma.hpp>
#include <modm/platform/clock/clockgen.hpp>

namespace modm::platform
{

void Dma::initialize()
{
	ClockGen::enable<ClockPeripheral::Xdmac>();
}

DmaChannel Dma::channel(uint_fast8_t index)
{
	return DmaChannel(index);
}

void Dma::enableInterruptVector(uint32_t priority)
{
    NVIC_SetPriority(XDMAC_IRQn, priority);
    NVIC_EnableIRQ(XDMAC_IRQn);
}

void Dma::disableInterruptVector()
{
	NVIC_DisableIRQ(XDMAC_IRQn);
}

} // namespace modm::platform
