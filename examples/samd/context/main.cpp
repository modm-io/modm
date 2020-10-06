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

#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>
#include <modm/board.hpp>

#define	MODM_LOG_LEVEL modm::log::INFO
#undef MODM_BOARD_HAS_LOGGER

using namespace Board;
using namespace std::chrono_literals;


modm_fastdata uint32_t stack1[64], stack2[64]; // 64 bytes stack
modm_fastdata modm_context f1_ctx, f2_ctx, m_ctx;

modm_naked
void f1() {
#ifdef MODM_BOARD_HAS_LOGGER
  MODM_LOG_INFO << "f1: entered" << modm::endl;
#endif
  while (1) {
    A0::set();
    modm_jumpcontext(&f1_ctx, f2_ctx);
  }
}

modm_naked
void f2() {
#ifdef MODM_BOARD_HAS_LOGGER
  MODM_LOG_INFO << "f2: entered" << modm::endl;
#endif
  while (1) {
    A0::reset();
    modm_jumpcontext(&f2_ctx, f1_ctx);
  }
}

// Frequency of A0 is 766.5kHz, resulting in ~36 CPU cycles per context switch (incl. overhead).
int main() {
  Board::initialize();
  A0::setOutput();
  f1_ctx = modm_makecontext((void**) stack1, sizeof(stack1), &f1, nullptr);
  f2_ctx = modm_makecontext((void**) stack2, sizeof(stack2), &f2, nullptr);
  MODM_LOG_DEBUG
    << "main: Jumping to f1 with sp-addr: "
    << modm::hex << f1_ctx.sp
    << " and f-addr: " << modm::hex << *(uint32_t*)(f1_ctx.sp + 0x10)
    << modm::endl;
  modm_jumpcontext(&m_ctx, f1_ctx);
  // Will never get here.
  return 0;
}