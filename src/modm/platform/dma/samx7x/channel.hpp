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

#ifndef MODM_SAMX7X_DMA_CHANNEL_HPP
#define MODM_SAMX7X_DMA_CHANNEL_HPP

#include <cstdint>
#include <modm/platform/dma/transfer.hpp>

namespace modm::platform
{

/**
 * DMA channel accessor.
 *
 * All data is stored in hardware or outside of this object. There is no
 * requirement to keep DmaChannel instances valid while the DMA is operating.
 *
 * Two different types of transfers are available: Single block transfers and
 * linked list transfers comprising multiple blocks with different configurations.
 *
 * @author Christopher Durand
 * @ingroup modm_platform_dma
 */
class DmaChannel : public DmaBase
{
public:
	/// @arg index DMA channel index (0..23)
	DmaChannel(uint8_t index);

	using Handler = void(*)(Interrupt_t);

	/// Start simple block transfer
	/// The currently active transfer will be stopped.
	void startTransfer(const BlockTransfer& transfer);

	/// Start linked-list transfer
	/// The currently active transfer will be stopped.
	/// @warning The linked list transfer object must remain valid for the whole duration of the transfer
	template<typename... Descriptors>
	void startTransfer(LinkedListTransfer<Descriptors...>& transfer);

	/// Abort current transfer
	void stopTransfer();

	/// Enable interrupts for channel
	void enableInterrupts();
	/// Disable interrupts for channel
	void disableInterrupts();

	void enableInterruptFlag(Interrupt_t interrupt);
	void disableInterruptFlag(Interrupt_t interrupt);

	/// Get enabled interrupt flags
	Interrupt_t interruptMask() const;

	Interrupt_t readAndClearFlags();

	/// Set handler for channel interrupts
	void setInterruptHandler(Handler handler);

private:
	void start();

	uint8_t index_{};
};

} // namespace modm::platform

#include "channel_impl.hpp"

#endif // MODM_SAMX7X_DMA_CHANNEL_HPP
