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

#include "fiber.hpp"

namespace modm::fiber
{

void yield();

class Scheduler
{
	friend class ::modm::Fiber;
	friend class Waitable;
	friend void yield();
	Scheduler(const Scheduler&) = delete;
	Scheduler() = delete;

protected:
	/// Last fiber in the ready queue.
	static inline Fiber* last{nullptr};
	/// Current running fiber
	static inline Fiber* current{nullptr};

public:
	// Should be called by the main() function.
	static inline bool
	run()
	{
		if (empty()) return false;
		current = last->next;
		modm_context_start(current->ctx.sp);
		return true;
	}

	static inline bool
	empty()
	{
		return last == nullptr;
	}

	static inline Fiber*
	removeCurrent()
	{
		if (current == last) last = nullptr;
		else last->next = current->next;
		current->next = nullptr;
		return current;
	}

	static inline void
	runNext(Fiber* fiber)
	{
		fiber->next = current->next;
		current->next = fiber;
	}

	static inline void
	runLast(Fiber* fiber)
	{
		fiber->next = last->next;
		last->next = fiber;
		last = fiber;
	}

protected:
	static inline void
	registerFiber(Fiber* fiber)
	{
		if (last == nullptr)
		{
			fiber->next = fiber;
			last = fiber;
			return;
		}
		runLast(fiber);
	}

	static inline void
	deregisterFiber()
	{
		Fiber* next = current->next;
		removeCurrent();
		if (empty())
		{
			current = nullptr;
			modm_context_end();
		}
		current->jump(*next);
	}
};

inline void
yield()
{
	if (Scheduler::current == nullptr) return;
	Fiber* next = Scheduler::current->next;
	if (next == Scheduler::current) return;
	Scheduler::last = Scheduler::current;
	Scheduler::current->jump(*next);
}

}  // namespace modm::fiber
