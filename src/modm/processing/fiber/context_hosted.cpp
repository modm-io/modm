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

#include "context.h"
#include <modm/architecture/interface/assert.hpp>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <pthread.h>

/* Every context is a thread with a stack from the operating system, and only
 * the thread of the active context of a scheduler is running. The fiber stack
 * only stores:
 *
 * Fiber Function
 * Fiber Function Argument
 */

extern "C"
{
constinit thread_local void *modm_context_scheduler{nullptr};
}

namespace
{

constexpr size_t StackWordsStorage = 2;
constexpr size_t StackSizeWord = sizeof(uintptr_t);

// Shared by the threads of a scheduler and lives in modm_context_start().
// With one condition variable every switch wakes all threads of the scheduler.
struct Group
{
	void *scheduler{modm_context_scheduler};
	modm_context_t *active{nullptr};
	// The thread of a fiber that ended and the context to continue with
	pthread_t ended;
	bool has_ended{false};
	modm_context_t *next{nullptr};
	std::mutex mutex;
	std::condition_variable wake;
};
constinit thread_local Group *group{nullptr};

// Leaves the fiber function from modm_context_end()
struct End
{
	modm_context_t *next;
};

void* entry(void *group_ptr);

// Must be called with the mutex locked
void
resume(Group &g, modm_context_t *to)
{
	g.active = to;
	// modm_context_reset() marks a context without a thread
	if (to and to->sp == to->top - StackWordsStorage)
	{
		to->sp++;
		pthread_t thread;
		const int error = pthread_create(&thread, nullptr, entry, &g);
		modm_assert(not error, "fbr.thrd", "Failed to create the fiber thread!", error);
	}
	g.wake.notify_all();
}

#ifdef __clang__
// The function pointer is stored untyped, exactly like on all other targets
__attribute__((no_sanitize("function")))
#endif
void*
entry(void *group_ptr)
{
	auto &g = *static_cast<Group*>(group_ptr);
	group = &g;
	modm_context_scheduler = g.scheduler;

	uintptr_t fn, arg;
	{
		// This thread was created for the active context
		std::lock_guard lock(g.mutex);
		fn = g.active->top[-1];
		arg = g.active->top[-2];
	}
	modm_context_t *next{nullptr};
	try { reinterpret_cast<void(*)(uintptr_t)>(fn)(arg); }
	catch (const End &end) { next = end.next; }
	// The scheduler already continues with the next fiber
	modm_context_scheduler = nullptr;

	// modm_context_start() joins this thread, so that the next fiber only
	// continues after all thread_local destructors of this one returned.
	std::lock_guard lock(g.mutex);
	g.ended = pthread_self();
	g.has_ended = true;
	g.next = next;
	g.wake.notify_all();
	return nullptr;
}

}

void
modm_context_init(modm_context_t *ctx,
				  uintptr_t *bottom, uintptr_t *top,
				  uintptr_t fn, uintptr_t fn_arg)
{
	ctx->bottom = bottom;
	ctx->top = top;

	ctx->sp = top;
	*--ctx->sp = fn;
	*--ctx->sp = fn_arg;
}

void
modm_context_reset(modm_context_t *ctx)
{
	ctx->sp = ctx->top - StackWordsStorage;
}

// The threads do not run on the fiber stack, so there is nothing to measure
void
modm_context_stack_watermark(modm_context_t *) {}

size_t
modm_context_stack_usage(const modm_context_t *)
{
	return 0;
}

size_t
modm_context_stack_size(const modm_context_t *ctx)
{
	return (ctx->top - ctx->bottom) * StackSizeWord;
}

uintptr_t
modm_context_start(modm_context_t *to)
{
	Group g;
	std::unique_lock lock(g.mutex);
	resume(g, to);
	while (g.active)
	{
		g.wake.wait(lock, [&]{ return g.has_ended; });
		g.has_ended = false;
		lock.unlock();
		pthread_join(g.ended, nullptr);
		lock.lock();
		resume(g, g.next);
	}
	return 0;
}

void
modm_context_jump(modm_context_t *from, modm_context_t *to)
{
	if (from == to)
	{
		// Other schedulers and threads may be the ones making progress
		std::this_thread::yield();
		return;
	}
	std::unique_lock lock(group->mutex);
	resume(*group, to);
	group->wake.wait(lock, [&]{ return group->active == from; });
}

// Ends the thread of the fiber and continues with the context in retval, or
// returns from modm_context_start() if it is zero.
void
modm_context_end(uintptr_t retval)
{
	throw End{reinterpret_cast<modm_context_t*>(retval)};
}
