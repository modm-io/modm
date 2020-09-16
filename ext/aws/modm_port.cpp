/*
 * Copyright (c) 2019-2020 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <modm/architecture/interface/assert.hpp>
#include <cstdlib>

extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char *);
void vApplicationStackOverflowHook(TaskHandle_t /*pxTask*/, char *pcTaskName)
{
	modm_assert(false, "freertos.stack", "FreeRTOS detected a stack overflow!", pcTaskName);
}

// ----------------------------------------------------------------------------
// Make the Newlib heap thread-safe with FreeRTOS

extern "C" void __malloc_lock(struct _reent *);
void __malloc_lock(struct _reent *)
{
	vTaskSuspendAll();
}

extern "C" void __malloc_unlock(struct _reent *);
void __malloc_unlock(struct _reent *)
{
	xTaskResumeAll();
}

// ----------------------------------------------------------------------------
// Make the FreeRTOS heap use Newlib heap

extern "C" void *pvPortMalloc(size_t xWantedSize);
void *pvPortMalloc(size_t xWantedSize)
{
	void *pvReturn = malloc(xWantedSize);
	traceMALLOC(pvReturn, xWantedSize);

#if configUSE_MALLOC_FAILED_HOOK == 1
	if(pvReturn == nullptr)
	{
		extern "C" void vApplicationMallocFailedHook(void);
		vApplicationMallocFailedHook();
	}
#endif

	return pvReturn;
}

extern "C" void vPortFree(void *pv);
void vPortFree(void *pv)
{
	if(pv)
	{
		free(pv);
		traceFREE(pv, 0);
	}
}
