/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;
using modm::OTA;

int
main()
{
    Board::initialize();
    OTA::initialize();
    OTA::write((uint32_t*) 0x10000u, 0x1000);
    OTA::apply();
    return 0;
}
