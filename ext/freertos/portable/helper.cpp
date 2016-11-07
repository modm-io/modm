/*
 * Copyright (c) 2011, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../FreeRTOS.h"
#include "../task.h"

// This function will get called if a task overflows its stack.
// If the parameters are corrupt then inspect pxCurrentTCB to find
// which was the offending task.
extern "C"
void
vApplicationStackOverflowHook(xTaskHandle *task, signed char *taskName)
{
	(void) task;
	(void) taskName;
	
	while (1)
		;
}
