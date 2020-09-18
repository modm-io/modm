/*
 * Copyright (c) 2019, Ethan Slattery
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing/timer.hpp>

using namespace Board;
using namespace std::chrono_literals;

int main() {
  Board::initialize();
  modm::Timeout timeout(1000ms);
  while (1) {
    Led::toggle();
    while (!timeout.isExpired())
      ;
    timeout.restart();

#ifdef MODM_BOARD_HAS_LOGGER
    static uint32_t counter(0);
    MODM_LOG_INFO << "Loop counter: " << (counter++) << modm::endl;
#endif
  }

  return 0;
}
