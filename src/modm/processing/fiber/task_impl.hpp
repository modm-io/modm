/*
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2021, 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include "scheduler.hpp"
#include <memory>

/// @cond
namespace modm::fiber
{

template<size_t Size, class T>
Task::Task(Stack<Size>& stack, T&& closure, Start start)
{
	constexpr bool with_stop_token = std::is_invocable_r_v<void, T, stop_token>;
	if constexpr (std::is_convertible_v<T, void(*)()> or
				  std::is_convertible_v<T, void(*)(stop_token)>)
	{
		// A plain function without closure
		using Callable = std::conditional_t<with_stop_token, void(*)(stop_token), void(*)()>;
		auto caller = (uintptr_t) +[](Callable fn)
		{
			if constexpr (with_stop_token) {
				fn(fiber::Scheduler::instance().current->get_stop_token());
			} else fn();
			fiber::Scheduler::instance().unschedule();
		};
		modm_context_init(&ctx, stack.memory, stack.memory + stack.words,
						  caller, (uintptr_t) static_cast<Callable>(closure));
	}
	else
	{
		// lambda functions with a closure must be allocated on the stack ALIGNED!
		constexpr size_t align_mask = std::max(StackAlignment, alignof(std::decay_t<T>)) - 1u;
		constexpr size_t closure_size = (sizeof(std::decay_t<T>) + align_mask) & ~align_mask;
		static_assert(Size >= closure_size + StackSizeMinimum,
				"Stack size must be larger than minimum stack size + aligned sizeof(closure))!");
		// Find a suitable aligned area at the top of stack to allocate the closure
		const uintptr_t ptr = uintptr_t(stack.memory + stack.words) - closure_size;
		// construct closure in place
		::new ((void*)ptr) std::decay_t<T>{std::forward<T>(closure)};
		// Encapsulate the proper ABI function call into a simpler function
		auto caller = (uintptr_t) +[](std::decay_t<T>* closure)
		{
			if constexpr (with_stop_token) {
				(*closure)(fiber::Scheduler::instance().current->get_stop_token());
			} else (*closure)();
			fiber::Scheduler::instance().unschedule();
		};
		// initialize the stack below the allocated closure
		modm_context_init(&ctx, stack.memory, (uintptr_t*)ptr, caller, ptr);
	}
	if (start == Start::Now) this->start();
}

bool inline
Task::start()
{
	if (isRunning()) return false;
	modm_context_reset(&ctx);
	stop.reset();
	Scheduler::instance().add(this);
	return true;
}

constexpr unsigned int
Task::hardware_concurrency()
{
	return Scheduler::hardware_concurrency();
}

bool inline
Task::joinable() const
{
	if (not isRunning()) return false;
	if (Scheduler::isInsideInterrupt()) return false;
	return get_id() != Scheduler::instance().get_id();
}

}
/// @endcond
