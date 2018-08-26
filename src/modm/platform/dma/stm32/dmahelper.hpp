/*
 * Copyright (c) 2018, Christopher Durand
 * Copyright (c) 2018, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_DMAHELPER_HPP
#define MODM_STM32_DMAHELPER_HPP

#include <stdint.h>
#include "dmahal_streambase.hpp"

/**
 * @ingroup 	{{partname}}
 * @defgroup	{{partname}}_dma DMA
 */


namespace modm::platform
{

// static class?
// erstmal nur als Ideensammlung anzusehen
class DmaHelper
{
public:
	using DmaHalStreamList = std::list<DmaHalStreamBase*>;

	static DmaHalStreamBase*
	acquire(DmaHalStreamList possiblestreams);

	static DmaHalStreamList
	filterStreamList(DmaHalStreamList possiblestreams, std::function<bool (DmaHalStreamBase*)> filter);
};

}	// namespace modm::platform

#endif	// MODM_STM32_DMAHELPER_HPP
