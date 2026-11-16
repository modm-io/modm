/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/can_message.hpp>
#include <algorithm>
#include <type_traits>
#include <stddef.h>
#include <stdint.h>

namespace modm::platform
{

/**
 * Software transmit queue of a CAN driver, sorted by arbitration priority.
 *
 * `get()` returns the frame that would win the bus arbitration first. Frames
 * with the same priority keep the order in which they were pushed, so the
 * frames of a multi-frame transfer with one identifier stay in order.
 *
 * Not thread-safe: the driver must serialize access from its TX interrupt.
 * push() and pop() move up to N indices, so keep the queue short (tens of frames).
 *
 * @ingroup	modm_platform_can_common
 */
template< size_t N >
class CanTxQueue
{
	static_assert(N > 0 and N < 65535, "Queue size must be 1..65534!");
	using Index = std::conditional_t<(N <= 255), uint8_t, uint16_t>;

public:
	/// Arbitration priority of a frame, lower values win.
	/// The arbitration field is the 11-bit base identifier, followed for
	/// standard frames by RTR and IDE (dominant), and for extended frames by
	/// SRR and IDE (recessive) and the 18-bit identifier extension. The RTR bit
	/// is ignored, so that data and remote frames with the same identifier
	/// also keep their order.
	static constexpr uint32_t
	priority(const modm::can::Message& message)
	{
		if (message.isExtended()) {
			const uint32_t id = message.getIdentifier() & 0x1fff'ffff;
			return ((id >> 18) << 20) | (1 << 19) | (id & 0x3'ffff);
		}
		return (message.getIdentifier() & 0x7ff) << 20;
	}

	CanTxQueue()
	{
		for (Index ii = 0; ii < N; ++ii) free_[ii] = ii;
	}

	bool isEmpty() const { return size_ == 0; }
	bool isNotEmpty() const { return size_ != 0; }
	bool isFull() const { return size_ == N; }
	bool isNotFull() const { return size_ != N; }
	size_t getSize() const { return size_; }
	static constexpr size_t getMaxSize() { return N; }

	/// Frame with the highest priority, the queue must not be empty.
	const modm::can::Message&
	get() const
	{
		return slots_[order_[0]];
	}

	/// Removes the frame with the highest priority, if any.
	void
	pop()
	{
		if (size_ == 0) return;
		free_[N - size_] = order_[0];
		size_--;
		std::ranges::copy(order_ + 1, order_ + size_ + 1, order_);
	}

	/// Inserts a frame behind all frames with higher or equal priority.
	bool
	push(const modm::can::Message& message)
	{
		if (size_ == N) return false;
		const Index slot = free_[N - 1 - size_];
		slots_[slot] = message;
		priorities_[slot] = priority(message);
		// behind all frames with higher or equal priority
		Index* const pos = std::ranges::upper_bound(order_, order_ + size_, priorities_[slot], {},
				[this](Index index) { return priorities_[index]; });
		std::ranges::copy_backward(pos, order_ + size_, order_ + size_ + 1);
		*pos = slot;
		size_++;
		return true;
	}

private:
	modm::can::Message slots_[N];
	uint32_t priorities_[N];
	Index order_[N];	// slots sorted by priority
	Index free_[N];		// free slots in free_[0 .. N - size_)
	Index size_{0};
};

} // namespace modm::platform
