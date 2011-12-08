
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
