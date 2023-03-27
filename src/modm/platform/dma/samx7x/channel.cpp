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

#include <array>
#include <atomic>
#include <bit>
#include <modm/platform/dma/channel.hpp>
#include <modm/architecture/interface/interrupt.hpp>

namespace
{
	using modm::platform::DmaChannel;

	std::array<std::atomic<DmaChannel::Handler>, DmaChannel::ChannelCount> handlers{};
	static_assert(std::atomic<DmaChannel::Handler>::is_always_lock_free);
}

MODM_ISR(XDMAC)
{
	uint32_t globalFlags = XDMAC->XDMAC_GIS & ((1u << DmaChannel::ChannelCount) - 1u);
	uint8_t channel = 0;

	while (globalFlags) {
		// find next set channel bit
		// std::countr_zero is 2 cycles on M4/M7 (RBIT + CLZ)
		const auto nextChannelOffset = std::countr_zero(globalFlags);
		channel += nextChannelOffset;
		globalFlags >>= (nextChannelOffset + 1);

		const auto channelFlags = XDMAC->XdmacChid[channel].XDMAC_CIS;

		auto* const handler = handlers[channel].load(std::memory_order_relaxed);
		if (handler) {
			using Interrupt_t = modm::platform::DmaBase::Interrupt_t;
			handler(Interrupt_t(channelFlags));
		}
		++channel;
	}
}

namespace modm::platform
{

DmaChannel::DmaChannel(uint8_t index)
	: index_{index}
{}

void DmaChannel::setInterruptHandler(Handler handler)
{
	handlers[index_].store(handler, std::memory_order_relaxed);
}

} // namespace modm::platform
