

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <modm/architecture/interface/assert.hpp>

extern "C" void vApplicationStackOverflowHook(TaskHandle_t, char *);

void vApplicationStackOverflowHook(TaskHandle_t /*pxTask*/, char *pcTaskName)
{
	modm_assert(false, "freertos.stack", "FreeRTOS detected a stack overflow!", pcTaskName);
}
