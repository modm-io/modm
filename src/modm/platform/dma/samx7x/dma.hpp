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

#ifndef MODM_SAMX7X_DMA_XDMAC_HPP
#define MODM_SAMX7X_DMA_XDMAC_HPP

#include <cstddef>
#include <cstdint>

#include <modm/platform/dma/channel.hpp>

namespace modm::platform
{

/// XDMAC DMA
/// initialize() must be called first before any other operation
/// @ingroup modm_platform_dma
class Dma : public DmaBase
{
public:
	/// Enable XDMAC clock. Must be called first.
	static void
	initialize();

	/// Get DMA channel accessor
	/// \param index DMA channel index (0..23)
	static DmaChannel
	channel(uint_fast8_t index);

	/// Enable interrupt vector. Must be enabled to receive any channel interrupts.
	/// \priority DMA interrupt priority
	static void
	enableInterruptVector(uint32_t priority);

	/// Disable interrupt vector. No channel interrupts will be active.
	static void
	disableInterruptVector();
};

} // namespace modm::platform

#endif // MODM_SAMX7X_DMA_XDMAC_HPP
