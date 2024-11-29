/*
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "macros.hpp"
#include <stdint.h>

#define MODM_RESUMABLE_IS_FIBER

namespace modm
{

/// @cond
// Reimplementations of modm::fiber::(recursive_)mutex without atomics
struct ResumableMutex
{
	bool locked{false};
	void inline lock() { while(locked) modm::this_fiber::yield(); locked = true; }
	void inline unlock() { locked = false; }
};
struct ResumableRecursiveMutex
{
	using count_t = uint16_t;
	static constexpr fiber::id NoOwner{fiber::id(-1)};
	fiber::id owner{NoOwner};
	static constexpr count_t countMax{count_t(-1)};
	count_t count{1};
	bool inline try_lock()
	{
		const auto id = modm::this_fiber::get_id();
		if (owner == NoOwner) { owner = id; return true; }
		if (owner == id and count < countMax) { count++; return true; }
		return false;
	}
	void inline lock() { while(not try_lock()) modm::this_fiber::yield(); }
	void inline unlock() { if (count > 1) count--; else owner = NoOwner; }
};
/// @endcond

/// @ingroup	modm_processing_resumable
/// @{

/// Resumable functions implemented via fibers return like a normal function
template < typename T >
using ResumableResult = T;

/// Resumable functions implemented via fibers are normal functions
template< uint8_t Functions = 1 >
class Resumable
{
public:
	bool isResumableRunning(uint8_t id) const
	{
		return id < Functions and rfStateArray[id].locked;
	}
	bool areAnyResumablesRunning() const
	{
		for (const auto &state : rfStateArray) if (state.locked) return true;
		return false;
	}
	bool areAnyResumablesRunning(std::initializer_list<uint8_t> ids) const
	{
		for (uint8_t id : ids) if (isResumableRunning(id)) return true;
		return false;
	}
	bool areAllResumablesRunning(std::initializer_list<uint8_t> ids) const
	{
		for (uint8_t id : ids) if (not isResumableRunning(id)) return false;
		return true;
	}
	bool joinResumables(std::initializer_list<uint8_t> ids) const
	{
		modm::this_fiber::poll([&]{ return not areAnyResumablesRunning(ids); });
		return true;
	}
	// unimplementable with fibers, but may be stubbed by user application
	void stopAllResumables();
	bool stopResumable(uint8_t id);

protected:
	/// @cond
	template<uint8_t index>
	static void
	checkRfFunctions()
	{
		static_assert(index < Functions,
				"Index out of bounds! Increase the `Functions` template argument of your Resumable class.");
	}
	template<bool isNested>
	static void
	checkRfType()
	{
		static_assert(isNested == false, "You must declare an index for this resumable function!");
	}
	ResumableMutex rfStateArray[Functions];
	/// @endcond
};

/// Resumable functions implemented via fibers are normal functions
template< uint8_t Levels = 1 >
class NestedResumable
{
public:
	bool isResumableRunning() const
	{
		return rfState.owner != rfState.NoOwner;
	}
	int8_t getResumableDepth() const
	{
		return isResumableRunning() ? rfState.count - 1 : -1;
	}
	// unimplementable with fibers, but may be stubbed by user application
	void stopResumable();

protected:
	/// @cond
	template<bool isNested>
	static void
	checkRfType()
	{
		static_assert(isNested == true, "You cannot declare an index for a _nested_ resumable function!");
	}

	ResumableRecursiveMutex rfState;
	/// @endcond
};

/// @}

} // namespace modm
