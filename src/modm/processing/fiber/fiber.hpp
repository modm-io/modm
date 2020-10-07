/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "context.hpp"

namespace modm
{

class Fiber;
inline void
yield();

namespace fiber
{

class Scheduler;

/**
 * Stack captures a memory area used as fiber stack.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
template<int size>
class Stack
{
	static_assert(size % 4 == 0, "Stack size must be multiple of 4.");
	static_assert(size >= 40, "Stack size must at least 40 bytes.");
	friend class ::modm::Fiber;
	friend class Scheduler;

public:
	Stack() = default;
	Stack(const Stack&) = delete;

private:
	modm_aligned(8) modm_stack_t memory_[size / sizeof(modm_stack_t)];
};

}  // namespace fiber

template<class Data_t>
class Channel;
class Waitable;

/**
 * Fiber is a cooperative threading model consisting of an execution context (stack) and a
 * function pointer representing the entry function.
 *
 * Fibers are scheduled in a round-robin fashion.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
class Fiber
{
	friend class fiber::Scheduler;
	template<class>
	friend class Channel;
	friend class Waitable;
	friend void
	yield();

public:
	template<int size, typename ReturnType>
	Fiber(fiber::Stack<size>& stack, ReturnType (*f)());

	/**
	 * Currently does nothing, but might be useful in the future for debugging.
	 */
	void
	dumpStack();

protected:
	inline void
	jump(Fiber& other);

private:
	Fiber() = default;
	Fiber(const Fiber&) = delete;

	// Removes the current fiber from the scheduler and yields execution. This is called on fiber
	// return.
	static inline void
	done();

	modm_always_inline Fiber*
	next()
	{
		return next_;
	}

	modm_always_inline void
	next(Fiber* fiber)
	{
		next_ = fiber;
	}

private:
	modm_context ctx_;
	modm_stack_t stack_;
	Fiber* next_;
};

/**
 * Waitable captures the low level fiber support for synchronization.
 *
 * Other higher level concepts such as channels, semaphores and mutexes are built on top of this.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
class Waitable
{
protected:
	/**
	 * Adds current fiber to the waitlist and yields execution.
	 */
	void
	wait();

	/**
	 * Resumes next fiber in the waitlist (yields execution of current fiber).
	 *
	 * This will push the waiting fiber to the front of the ready queue, the reason for this is that
	 * it allows a more efficient implementation of message passing (the reciever is guaranteed to
	 * consume the sent message immediately).
	 */
	void
	signal();

private:
	Fiber* last_waiter_;
	Fiber*
	popWaiter();
	void
	pushWaiter(Fiber* waiter);
};

/**
 * A Channel allows message passing between fibers.
 *
 * The channel can have three states:
 *
 * 1. Empty: Polling fibers will wait for messages to become available.
 * 2. Full: Pushing threads will wait for buffer space to become available.
 * 3. Ready: There are no waiting fibers. All operations are non-blocking.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
template<typename Data_t>
class Channel : Waitable
{
public:
	bool
	empty()
	{
		return !size_;
	}

	bool
	full()
	{
		return size_ > buffer_size_;
	}

	/**
	 * Send data to the channel.
	 *
	 * This method will be non-blocking if the channel is in the ready state, otherwise it will
	 * yield execution.
	 *
	 *  @param data	Data to add to the channel
	 */
	void
	send(const Data_t& data);

	/**
	 * Receive data from the channel.
	 *
	 * This method will be non-blocking if the channel is in the ready state, otherwise it will
	 * yield execution.
	 *
	 *  @return The data from the channel.
	 */
	Data_t
	recv();

private:
	Data_t data_;
	Data_t* buffer_ = nullptr;
	size_t buffer_size_ = 0;
	size_t size_;
};

/**
 * A Semaphore is a lightweight synchronization primitive that can control access to a shared
 * resource. It allows up to MaxValue fibers to access the resource concurrently.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
template<std::ptrdiff_t MaxValue>
class Semaphore : Waitable
{
public:
	/**
	 * Increments the internal counter and unblocks acquirers.
	 *
	 * Yields execution if the counter is equal to MaxValue.
	 */
	void
	release()
	{
		if (!counter_) { wait(); }
		++counter_;
		signal();
	}

	/**
	 * Decrements the internal counter and unblocks releasers.
	 *
	 * Yields execution if the counter is zero.
	 */
	void
	acquire()
	{
		if (counter_ == max()) { wait(); }
		++counter_;
		signal();
	}

	static std::ptrdiff_t
	max()
	{
		return MaxValue;
	}

private:
	std::ptrdiff_t counter_ = 0;
};

/**
 * A Mutex is a Semaphore which allows only one fiber to enter at a time.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
using Mutex = Semaphore<1>;

/**
 * A MutexLock is a helper class that acquires/releases the Mutex via RAII.
 *
 * @author	Erik Henriksson
 * @ingroup	modm_processing_fiber
 */
class MutexLock
{
public:
	/**
	 * Aquires the Mutex.
	 */
	MutexLock(Mutex* m) : mutex_(m) { mutex_->acquire(); }

	/**
	 * Releases the Mutex.
	 */
	~MutexLock() { mutex_->release(); }

private:
	Mutex* mutex_;
};

namespace fiber
{

class Scheduler
{
	friend class modm::Fiber;
	friend void
	modm::yield();

public:
	constexpr Scheduler() = default;
	/* Should be called by the main() function. */
	inline void
	start();

	inline Fiber*
	currentFiber()
	{
		return current_fiber_;
	}

	inline Fiber*
	lastFiber()
	{
		return last_fiber_;
	}

	inline bool
	empty()
	{
		return last_fiber_ == nullptr;
	}

	inline Fiber*
	removeCurrent()
	{
		Fiber* current = currentFiber();
		if (current == last_fiber_)
		{
			last_fiber_ = nullptr;
		} else
		{
			last_fiber_->next(current->next());
		}
		current->next(nullptr);
		return current;
	}

	inline void
	runNext(Fiber* fiber)
	{
		Fiber* current = currentFiber();
		fiber->next(current->next());
		current->next(fiber);
	}

	inline void
	runLast(Fiber* fiber)
	{
		fiber->next(last_fiber_->next());
		last_fiber_->next(fiber);
		last_fiber_ = fiber;
	}

protected:
	inline void
	registerFiber(Fiber*);

private:
	Scheduler(const Scheduler&) = delete;
	// Last fiber in the ready queue.
	Fiber* last_fiber_ = nullptr;
	// Current running fiber
	Fiber* current_fiber_ = nullptr;
};

extern Scheduler scheduler;

}  // namespace fiber

}  // namespace modm

#include "fiber_impl.hpp"
