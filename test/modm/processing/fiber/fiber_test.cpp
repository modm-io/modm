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

#include "fiber_test.hpp"

#define MODM_BOARD_HAS_LOGGER
#include <modm/processing/fiber.hpp>

#include <modm/debug/logger.hpp>

#include <array>
#include <string>
#include <vector>

namespace {

enum State {
  INVALID,
  F1_START,
  F1_END,
  F2_START,
  F2_END,
  F3_START,
  F3_END,
  SUBROUTINE_START,
  SUBROUTINE_END,
  CONSUMER_START,
  CONSUMER_END,
  PRODUCER_START,
  PRODUCER_END,
};

std::array<State, 6> states = {};
size_t states_pos = 0;

#define ADD_STATE(state) \
  states[states_pos++] = state;


void f1() {
  ADD_STATE(F1_START);
  modm::yield();
  ADD_STATE(F1_END);
}

void f2() {
  ADD_STATE(F2_START);
  modm::yield();
  ADD_STATE(F2_END);
}

modm::fiber::Stack<1024> stack1, stack2;

}

__attribute__((noinline))
void subroutine() {
  ADD_STATE(SUBROUTINE_START);
  modm::yield();
  ADD_STATE(SUBROUTINE_END);
}

void FiberTest::testOneFiber() {
  MODM_LOG_INFO << "testOneFiber" << modm::endl;
  states_pos = 0;
  modm::Fiber fiber(stack1, &f1);
  modm::fiber::scheduler.start();
  TEST_ASSERT_EQUALS(states_pos, 2u);
  TEST_ASSERT_EQUALS(states[0], F1_START);
  TEST_ASSERT_EQUALS(states[1], F1_END);
}

void FiberTest::testTwoFibers() {
  MODM_LOG_INFO << "testTwoFibers" << modm::endl;
  states_pos = 0;
  modm::Fiber fiber1(stack1, &f1), fiber2(stack2, &f2);
  modm::fiber::scheduler.start();
  TEST_ASSERT_EQUALS(states_pos, 4u);
  TEST_ASSERT_EQUALS(states[0], F1_START);
  TEST_ASSERT_EQUALS(states[1], F2_START);
  TEST_ASSERT_EQUALS(states[2], F1_END);
  TEST_ASSERT_EQUALS(states[3], F2_END);
}

namespace {

void f3() {
  ADD_STATE(F3_START);
  subroutine();
  ADD_STATE(F3_END);
}

} // namespace

void FiberTest::testYieldFromSubroutine() {
  MODM_LOG_INFO << "testYieldFromSubroutine" << modm::endl;
  states_pos = 0;
  modm::Fiber fiber1(stack1, &f1), fiber2(stack2, &f3);
  modm::fiber::scheduler.start();
  TEST_ASSERT_EQUALS(states_pos, 6u);
  TEST_ASSERT_EQUALS(states[0], F1_START);
  TEST_ASSERT_EQUALS(states[1], F3_START);
  TEST_ASSERT_EQUALS(states[2], SUBROUTINE_START);
  TEST_ASSERT_EQUALS(states[3], F1_END);
  TEST_ASSERT_EQUALS(states[4], SUBROUTINE_END);
  TEST_ASSERT_EQUALS(states[5], F3_END);
}

namespace {

modm::Channel<int> channel;

void consumer() {
  ADD_STATE(CONSUMER_START);
  TEST_ASSERT_EQUALS(channel.recv(), 123);
  ADD_STATE(CONSUMER_END);
}

void producer() {
  ADD_STATE(PRODUCER_START);
  channel.send(123);
  ADD_STATE(PRODUCER_END);
}

} // namespace

void FiberTest::testBlockingRecieve() {
  MODM_LOG_INFO << "testBlockingRecieve" << modm::endl;
  states_pos = 0;
  modm::Fiber fiber1(stack1, &consumer), fiber2(stack2, &producer);
  modm::fiber::scheduler.start();
  TEST_ASSERT_EQUALS(states_pos, 4u);
  TEST_ASSERT_EQUALS(states[0], CONSUMER_START);
  TEST_ASSERT_EQUALS(states[1], PRODUCER_START);
  TEST_ASSERT_EQUALS(states[2], CONSUMER_END);
  TEST_ASSERT_EQUALS(states[3], PRODUCER_END);
}

void FiberTest::testNonBlockingRecieve() {
  MODM_LOG_INFO << "testNonBlockingRecieve" << modm::endl;
  states_pos = 0;
  modm::Fiber fiber1(stack1, &producer), fiber2(stack2, &consumer);
  modm::fiber::scheduler.start();
  TEST_ASSERT_EQUALS(states_pos, 4u);
  TEST_ASSERT_EQUALS(states[0], PRODUCER_START);
  TEST_ASSERT_EQUALS(states[1], PRODUCER_END);
  TEST_ASSERT_EQUALS(states[2], CONSUMER_START);
  TEST_ASSERT_EQUALS(states[3], CONSUMER_END);
}

namespace {

modm::Semaphore<1> semaphore;

void semaphoreProducer() {
  ADD_STATE(PRODUCER_START);
  semaphore.acquire();
  ADD_STATE(PRODUCER_END);
}

void semaphoreConsumer() {
  ADD_STATE(CONSUMER_START);
  semaphore.release();
  ADD_STATE(CONSUMER_END);
}

} // namespace


void FiberTest::testSemaphore() {
  MODM_LOG_INFO << "testSemaphore" << modm::endl;
  states_pos = 0;
  modm::Fiber fiber1(stack1, &semaphoreProducer), fiber2(stack2, &semaphoreConsumer);
  modm::fiber::scheduler.start();
  TEST_ASSERT_EQUALS(states_pos, 4u);
  TEST_ASSERT_EQUALS(states[0], PRODUCER_START);
  TEST_ASSERT_EQUALS(states[1], PRODUCER_END);
  TEST_ASSERT_EQUALS(states[2], CONSUMER_START);
  TEST_ASSERT_EQUALS(states[3], CONSUMER_END);
}