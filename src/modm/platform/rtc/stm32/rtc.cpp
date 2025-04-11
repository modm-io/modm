/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "rtc.hpp"

extern "C" int
_gettimeofday(struct timeval *tp, void *)
{
	*tp = modm::platform::Rtc::timeval();
	return 0;
}
