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

#include "fiber.hpp"

namespace modm {

namespace fiber {

Scheduler scheduler;

} // namespace fiber

void Waitable::wait() {
  using ::modm::fiber::scheduler;
  pushWaiter(scheduler.removeCurrent());
  scheduler.currentFiber()->jump(*scheduler.lastFiber()->next());
}

void Waitable::signal() {
  using ::modm::fiber::scheduler;
  Fiber* waiter = popWaiter();
  if (waiter != nullptr) {
    scheduler.runNext(waiter);
    yield();
  }
}

Fiber* Waitable::popWaiter() {
  if (!last_waiter_) {
    return nullptr;
  }
  Fiber* first = last_waiter_->next();
  if (first == last_waiter_) {
    last_waiter_ = nullptr;
  } else {
    last_waiter_->next(first->next());
  }
  first->next(nullptr);
  return first;
}

void Waitable::pushWaiter(Fiber* waiter) {
  if (last_waiter_) {
    waiter->next(last_waiter_->next());
    last_waiter_->next(waiter);
  } else {
    waiter->next(waiter);
  }
  last_waiter_ = waiter;
}

} // namespace modm
