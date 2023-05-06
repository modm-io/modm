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

#include "stack.hpp"
#include "context.h"
#include <type_traits>

namespace modm::fiber
{

class Scheduler;

/// The Fiber scheduling policy.
/// @ingroup modm_processing_fiber
enum class
Start
{
	Now,	// Automatically add the fiber to the active scheduler.
	Later,	// Manually add the fiber to a scheduler.
};

/**
 * The fiber task connects the callable fiber object with the fiber context and
 * scheduler. It constructs the fiber function on the stack if necessary, and
 * adds the contexts to the scheduler. If the fiber function returns, the task
 * is removed from the scheduler. Tasks can then be restarted, which will call
 * the fiber function from the beginning again
 *
 * Note that a task contains no stack, only the control structures necessary for
 * managing a fiber. You may therefore place objects of this class in fast
 * core-local memory, while the stack must remain in DMA-able memory!
 *
 * @author Erik Henriksson
 * @author Niklas Hauser
 * @ingroup modm_processing_fiber
 */
class Task
{
	Task() = delete;
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;
	friend class Scheduler;

	// Make sure that Task and Fiber use a callable constructor, otherwise they
	// may get placed in the .data section including the whole stack!!!
	modm_context_t ctx;
	Task* next;
	Scheduler *scheduler{nullptr};

public:
	/// @param stack	A stack object that is *NOT* shared with other tasks.
	/// @param closure	A callable object of signature `void(*)()`.
	/// @param start	When to start this task.
	template<size_t Size, class T>
	Task(Stack<Size>& stack, T&& closure, Start start=Start::Now);

	/// Watermarks the stack to measure `stack_usage()` later.
	/// @see `modm_context_watermark()`.
	void
	watermark_stack()
	{
		modm_context_watermark(&ctx);
	}

	/// @returns the stack usage as measured by a watermark level.
	/// @see `modm_context_stack_usage()`.
	size_t
	stack_usage() const
	{
		return modm_context_stack_usage(&ctx);
	}

	/// @returns if the bottom word on the stack has been overwritten.
	/// @see `modm_context_stack_overflow()`.
	bool
	stack_overflow() const
	{
		return modm_context_stack_overflow(&ctx);
	}

	/// Adds the task to the currently active scheduler, if not already running.
	/// @returns if the fiber has been scheduled.
	bool
	start();

	/// @returns if the fiber is attached to a scheduler.
	bool
	isRunning() const
	{
		return scheduler;
	}
};

}	// namespace modm::fiber

#include "scheduler.hpp"
#include <memory>

/// @cond
namespace modm::fiber
{

template<size_t Size, class T>
Task::Task(Stack<Size>& stack, T&& closure, Start start)
{
	if constexpr (std::is_convertible_v<T, void(*)()>)
	{
		// A plain function without closure
		auto caller = (uintptr_t) +[](void(*fn)())
		{
			fn();
			fiber::Scheduler::instance().unschedule();
		};
		modm_context_init(&ctx, stack.memory, stack.memory + stack.words,
						  caller, (uintptr_t) static_cast<void(*)()>(closure));
	}
	else
	{
		// lambda functions with a closure must be allocated on the stack
		constexpr size_t closure_size = sizeof(std::decay_t<T>);
		static_assert(Size >= closure_size + StackSizeMinimum,
					  "Stack size must ≥({{min_stack_size}}B + sizeof(closure))!");
		// Find a suitable aligned area at the top of stack to allocate the closure
		uintptr_t ptr = uintptr_t(stack.memory + stack.words) - closure_size;
		ptr &= ~(std::max(sizeof(uintptr_t), alignof(std::decay_t<T>)) - 1u);
		// construct closure in place
		::new ((void*)ptr) std::decay_t<T>{std::forward<T>(closure)};
		// Encapsulate the proper ABI function call into a simpler function
		auto caller = (uintptr_t) +[](std::decay_t<T>* closure)
		{
			(*closure)();
			fiber::Scheduler::instance().unschedule();
		};
		// initialize the stack below the allocated closure
		modm_context_init(&ctx, stack.memory, (uintptr_t*)ptr, caller, ptr);
	}
	if (start == Start::Now) this->start();
}

inline bool
Task::start()
{
	if (isRunning()) return false;
	modm_context_reset(&ctx);
	fiber::Scheduler::instance().add(*this);
	return true;
}

}
/// @endcond
