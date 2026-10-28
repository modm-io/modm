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

template<size_t Size, class T, class... Args>
Task::Task(Stack<Size>& stack, T&& closure, Start start, Args&&... args)
{
	emplace(stack, std::forward<T>(closure), std::forward<Args>(args)...);
	if (start == Start::Now) this->start();
}

template<size_t Size, class T, class... Args>
void
Task::emplace(Stack<Size>& stack, T&& closure, Args&&... args)
{
	if constexpr (sizeof...(Args))
	{
		// Bind the arguments to the closure, they are passed as lvalues, since
		// they must remain valid for restarting the task
		constexpr bool with_stop_token = std::is_invocable_v<std::decay_t<T>&,
				stop_token, std::decay_t<Args>&...>;
		static_assert(with_stop_token or std::is_invocable_v<std::decay_t<T>&, std::decay_t<Args>&...>,
				"Closure must be callable with the arguments as lvalues!");
		emplace(stack, [closure = std::forward<T>(closure), ...args = std::forward<Args>(args)]
					   (stop_token stoken) mutable
		{
			if constexpr (with_stop_token) closure(stoken, args...);
			else { (void) stoken; closure(args...); }
		});
	}
	else
	{
		static_assert(std::is_invocable_v<std::decay_t<T>&> or
					  std::is_invocable_v<std::decay_t<T>&, stop_token>,
				"Closure must be callable as void() or void(stop_token), "
				"arguments can only be given together with a closure!");
		if (destroy) destroy(ctx.top);
		destroy = nullptr;

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
			using Closure = std::decay_t<T>;
			// lambda functions with a closure must be allocated on the stack ALIGNED!
			constexpr size_t align_mask = std::max(StackAlignment, alignof(Closure)) - 1u;
			constexpr size_t closure_size = (sizeof(Closure) + align_mask) & ~align_mask;
			static_assert(Size >= closure_size + StackSizeMinimum,
					"Stack size must be larger than minimum stack size + aligned sizeof(closure))!");
			// Find a suitable aligned area at the top of stack to allocate the closure
			const uintptr_t ptr = uintptr_t(stack.memory + stack.words) - closure_size;
			// construct closure in place
			::new ((void*)ptr) Closure{std::forward<T>(closure)};
			if constexpr (not std::is_trivially_destructible_v<Closure>)
				destroy = +[](uintptr_t* closure) { std::destroy_at((Closure*) closure); };
			// Encapsulate the proper ABI function call into a simpler function
			auto caller = (uintptr_t) +[](Closure* closure)
			{
				if constexpr (with_stop_token) {
					(*closure)(fiber::Scheduler::instance().current->get_stop_token());
				} else (*closure)();
				fiber::Scheduler::instance().unschedule();
			};
			// initialize the stack below the allocated closure
			modm_context_init(&ctx, stack.memory, (uintptr_t*)ptr, caller, ptr);
		}
	}
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
