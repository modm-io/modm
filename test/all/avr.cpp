/*
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/platform.hpp>
#include <modm/architecture/interface/delay.hpp>
using namespace std::chrono_literals;

int main(void)
{
    using LedRed = modm::platform::GpioOutputB0;
    LedRed::setOutput();

    while (true)
    {
        LedRed::toggle();
        modm::delay(500ms);
    }

    return 0;
}
