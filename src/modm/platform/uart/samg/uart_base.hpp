/*
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include "../device.hpp"

namespace modm::platform
{

class UartBase
{
public:
    enum class Parity : uint32_t
    {
        Disabled 	= US_MR_PAR_NO,
        Even 		= US_MR_PAR_EVEN,
        Odd  		= US_MR_PAR_ODD,
        Space		= US_MR_PAR_SPACE,
        Mark		= US_MR_PAR_MARK,
        MultiDrop	= US_MR_PAR_MULTIDROP
    };

    enum class WordLength : uint32_t
    {
        Bit5 = 0,
        Bit6,
        Bit7,
        Bit8,
        Bit9
    };
};

} // namespace modm::platform
