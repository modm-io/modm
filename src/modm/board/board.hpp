/*
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "board/board.hpp"

#ifndef MODM_BOARD_DO_NOT_USE_LITERALS
using namespace modm::literals;
using namespace std::chrono_literals;
#endif

namespace Board {
    // DEPRECATE: 2022q1
	using systemClock [[deprecated("Use `Board::SystemClock` instead!")]] = SystemClock;
}
