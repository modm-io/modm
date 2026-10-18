/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2016-2017, 2026, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ATOMIC_QUEUE_HPP
#define	MODM_ATOMIC_QUEUE_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace modm::atomic
{

/**
 * Interrupt-safe single-producer single-consumer queue.
 *
 * Exchanges data between exactly one producer context, which calls `push()`,
 * and exactly one consumer context, which calls `get()` and `pop()`, for
 * example between an interrupt and the main loop.
 *
 * The producer only writes the head index and the consumer only writes the
 * tail index. Both are `std::atomic`, so that the indices are never torn and
 * that an element is completely written before the head index makes it
 * visible to the consumer (release/acquire). On AVR the 16-bit indices for
 * queues with more than 254 elements are accessed with interrupts disabled.
 *
 * @warning	Multiple producers or multiple consumers must be serialized
 *			externally, for example with `modm::atomic::Lock`.
 *
 * @tparam	T	Type of the elements
 * @tparam	N	Capacity of the queue, must be larger than three
 *
 * @ingroup	modm_architecture_atomic
 */
template<typename T, std::size_t N>
class Queue
{
	static_assert(N > 3, "The queue must be able to store more than three elements.");

public:
	using Index = std::conditional_t< (N >= 255), uint16_t, uint8_t >;
	using Size = Index;

	constexpr Queue() = default;

	Queue(const Queue&) = delete;
	Queue& operator=(const Queue&) = delete;

	/// Can be called from both contexts.
	bool
	isFull() const
	{ return next(head.load(std::memory_order_relaxed)) == tail.load(std::memory_order_acquire); }

	/// Can be called from both contexts.
	bool
	isNotFull() const
	{ return not isFull(); }

	/// @returns `true` if less than three elements can be stored in the queue.
	bool
	isNearlyFull() const
	{
		return getSize() > (N - 3);
	}

	/// Can be called from both contexts.
	bool
	isEmpty() const
	{ return tail.load(std::memory_order_relaxed) == head.load(std::memory_order_acquire); }

	/// Can be called from both contexts.
	bool
	isNotEmpty() const
	{ return not isEmpty(); }

	/// @returns `true` if less than three elements are stored in the queue.
	bool
	isNearlyEmpty() const
	{
		return getSize() < 3;
	}

	static constexpr Size
	getMaxSize()
	{ return N; }

	/// The size is only a snapshot, since the other context may change it.
	Size
	getSize() const
	{
		const Index h = head.load(std::memory_order_acquire);
		const Index t = tail.load(std::memory_order_acquire);
		return (h >= t) ? (h - t) : (N + 1 - t + h);
	}

	/// Consumer only: access the oldest element.
	/// @warning	Undefined behavior if the queue is empty.
	const T&
	get() const
	{ return buffer[tail.load(std::memory_order_relaxed)]; }

	/// Producer only: append an element.
	/// @returns `false` if the queue is full.
	bool
	push(const T& value)
	{
		const Index h = head.load(std::memory_order_relaxed);
		const Index n = next(h);
		// acquire: the consumer must be done with the element before it is overwritten
		if (n == tail.load(std::memory_order_acquire)) return false;
		buffer[h] = value;
		// release: the element must be written before the consumer can see it
		head.store(n, std::memory_order_release);
		return true;
	}

	/// Consumer only: remove the oldest element.
	/// @warning	Undefined behavior if the queue is empty.
	void
	pop()
	{
		const Index t = tail.load(std::memory_order_relaxed);
		// release: the element must be read before the producer can overwrite it
		tail.store(next(t), std::memory_order_release);
	}

private:
	static constexpr Index
	next(Index index)
	{ return (index >= N) ? 0 : index + 1; }

	std::atomic<Index> head{0};	///< written only by the producer
	std::atomic<Index> tail{0};	///< written only by the consumer
	T buffer[N + 1]{};
};

}	// namespace modm::atomic

#endif	// MODM_ATOMIC_QUEUE_HPP
