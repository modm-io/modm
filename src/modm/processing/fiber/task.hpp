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

#include "context.h"
#include "stack.hpp"
#include "stop_token.hpp"
#include <modm/architecture/interface/fiber.hpp>
#include <type_traits>

namespace modm
{

namespace fiber
{

// forward declaration
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
 * @see https://en.cppreference.com/w/cpp/thread/jthread
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
	stop_state stop{};

public:
	/// @param stack	A stack object that is *NOT* shared with other tasks.
	/// @param closure	A callable object of signature `void()`.
	/// @param start	When to start this task.
	template<size_t Size, class Callable>
	Task(Stack<Size>& stack, Callable&& closure, Start start=Start::Now);

	inline
	~Task()
	{
		request_stop();
		join();
	}

	/// Returns the number of concurrent threads supported by the implementation.
	[[nodiscard]] static constexpr unsigned int
	hardware_concurrency();

	/// Returns a value of std::thread::id identifying the thread associated
	/// with `*this`.
	/// @note This function can be called from an interrupt.
	[[nodiscard]] modm::fiber::id inline
	get_id() const
	{
		return reinterpret_cast<id>(this);
	}

	/// Checks if the Task object identifies an active fiber of execution.
	[[nodiscard]] bool
	joinable() const;

	/// Blocks the current fiber until the fiber identified by `*this`
	/// finishes its execution. Returns immediately if the thread is not joinable.
	void inline
	join()
	{
		if (joinable()) while(isRunning()) modm::this_fiber::yield();
	}

	[[nodiscard]]
	stop_source inline
	get_stop_source()
	{
		return stop.get_source();
	}

	[[nodiscard]]
	stop_token inline
	get_stop_token()
	{
		return stop.get_token();
	}

	/// @note This function can be called from an interrupt.
	bool inline
	request_stop()
	{
		return stop.request_stop();
	}


	/// Watermarks the stack to measure `stack_usage()` later.
	/// @see `modm_context_stack_watermark()`.
	void inline
	stack_watermark()
	{
		modm_context_stack_watermark(&ctx);
	}

	/// @returns the stack usage as measured by a watermark level.
	/// @see `modm_context_stack_usage()`.
	[[nodiscard]] size_t inline
	stack_usage() const
	{
		return modm_context_stack_usage(&ctx);
	}

	/// Adds the task to the currently active scheduler, if not already running.
	/// @returns if the fiber has been scheduled.
	bool
	start();

	/// @returns if the fiber is attached to a scheduler.
	[[nodiscard]] bool inline
	isRunning() const
	{
		return scheduler;
	}

	/// @cond
	// DEPRECATE: 2025q4
	[[deprecated("Use `stack_watermark()` instead!")]]
	void inline watermark_stack() { stack_watermark(); }
	/// @endcond
};

}	// namespace modm::fiber

/**
 * This class is a convenience extension of a fiber task, which contains the
 * `modm::fiber::Stack` as part of the class. The class is constructed at
 * runtime only, therefore it is placed into the `.bss` section by default and
 * does not count towards your `.data` section size. To speed up booting you
 * may place the object into the `modm_faststack` section.
 *
 * @ingroup modm_processing_fiber
 */
template< size_t StackSize = fiber::StackSizeDefault >
class Fiber : public fiber::Task
{
	fiber::Stack<StackSize> stack;
public:
	template<class T>
	Fiber(T&& task, fiber::Start start=fiber::Start::Now)
	: Task(stack, std::forward<T>(task), start)
	{}
};

}	// namespace modm

// carefully avoid incomplete type issues
#ifndef MODM_FIBER_SCHEDULER_HPP
#include "task_impl.hpp"
#endif
