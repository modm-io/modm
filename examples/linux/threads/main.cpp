
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/rtos.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
/*xpcc::rtos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class LedTask1 : public xpcc::rtos::Task
{
public:
	LedTask1() :
		xpcc::rtos::Task(2)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			// synchronize with LedTask2
			event.release();
			
			this->sleep(1000 * MILLISECONDS);
		}
	}
};

// Toggle Led2 when LedTask1 starts a new cycle
class LedTask2 : public xpcc::rtos::Task
{
public:
	LedTask2() :
		xpcc::rtos::Task(2)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			
			// wait for the other task
			event.acquire();
		}
	}
};*/

class LoggingTask : public xpcc::rtos::Task
{
public:
	LoggingTask() :
		xpcc::rtos::Task(0)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			XPCC_LOG_DEBUG << "Hello World!" << xpcc::endl;
			
			this->sleep(2000);
		}
	}
};

// Create the three Tasks. They are automatically added to the task list
// and started when the FreeRTOS scheduler is called.
//LedTask1 task1;
//LedTask2 task2;
LoggingTask task3;

int
main(void)
{
	
	// start scheduler
	xpcc::rtos::Scheduler::schedule();	
	
	return 0;
}
