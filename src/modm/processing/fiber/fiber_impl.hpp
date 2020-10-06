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

// #include <modm/architecture/interface/assert.hpp>

#include <utility>

namespace modm
{

/*
 * Important: Since this function is using callee's stack, you should not use any stack. That means
 * all function calls need to be inlined and all variables register allocated.
 */
void yield()
{
  using fiber::scheduler;
  Fiber* current = scheduler.currentFiber();
  Fiber* next = current->next();
  if (next == current) return;
  scheduler.last_fiber_ = current;
  current->jump(*next);
}

template<int size, typename ReturnType>
Fiber::Fiber(fiber::Stack<size>& stack, ReturnType(*f)())
	: stack_(stack.memory_)
{
	ctx_ = modm_makecontext(stack.memory_, sizeof(stack.memory_), (void(*)(void)) f, done);
	fiber::scheduler.registerFiber(this);
}

void
Fiber::jump(Fiber& other)
{
  fiber::scheduler.current_fiber_ = &other;
  modm_jumpcontext(&ctx_, other.ctx_);
}

void Fiber::done() {
  using fiber::scheduler;
  Fiber* current = scheduler.currentFiber();
  Fiber* next = current->next();
  scheduler.removeCurrent();
  if (scheduler.empty()) {
    scheduler.current_fiber_ = nullptr;
    modm_endcontext();
  }
  current->jump(*next);
}

template<class Data_t>
void Channel<Data_t>::send(const Data_t& data) {
  if (full()) {
    wait();
  }
  // Now we are in empty or ready state.
  // modm_assert(!full(), "Channel::push.full", "Channel should not be full");
  if (empty()) {
    data_ = data;
  } else {
    buffer_[size_ - 1] = data;
  }
  ++size_;
  signal();
}

template<class Data_t>
Data_t Channel<Data_t>::recv() {
  if (empty()) {
    wait();
  }
  // Now we are in full or ready state.
  // modm_assert(!empty(), "Channel::recv.empty", "Channel should not be empty");
  Data_t result;
  if (--size_) {
    result = std::move(buffer_[size_]);
  } else {
    result = std::move(data_);
  }
  signal();
  return result;
}

namespace fiber {

void Scheduler::registerFiber(Fiber* fiber) {
  if (last_fiber_ == nullptr) {
    fiber->next(fiber);
    last_fiber_ = fiber;
    return;
  }
  runLast(fiber);
}

void Scheduler::start() {
  if (last_fiber_ == nullptr) return;
  current_fiber_ = last_fiber_->next();
  modm_startcontext(currentFiber()->ctx_);
}

}

} // namespace modm
