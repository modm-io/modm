/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "error_report.hpp"

static void
dummyErrorHandler(uint16_t)
{
}

// ----------------------------------------------------------------------------
modm::ErrorReport::Handler modm::ErrorReport::globalErrorHandler = &dummyErrorHandler;

// ----------------------------------------------------------------------------
void
modm::ErrorReport::attach(Handler handler)
{
	globalErrorHandler = handler;
}

void
modm::ErrorReport::detach()
{
	globalErrorHandler = &dummyErrorHandler;
}
