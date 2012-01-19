
#include <xpcc/architecture.hpp>
#include <xpcc/workflow/rtos.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
xpcc::rtos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class Task1 : public xpcc::rtos::Task
{
public:
	Task1() :
		xpcc::rtos::Task(2)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			XPCC_LOG_DEBUG << "ping" << xpcc::endl;
			
			// synchronize with Task2
			event.release();
			
			this->sleep(1000 * MILLISECONDS);
		}
	}
};

// Toggle Led2 when LedTask1 starts a new cycle
class Task2 : public xpcc::rtos::Task
{
public:
	Task2() :
		xpcc::rtos::Task(2)
	{
	}
	
	virtual void
	run()
	{
		while (1)
		{
			XPCC_LOG_DEBUG << "pong" << xpcc::endl;
			
			// wait for the other task
			event.acquire();
		}
	}
};

// Create the two Tasks. They are automatically added to the task list
// and started when the Scheduler is called.
Task1 task1;
Task2 task2;

int
main(void)
{
	// start scheduler
	xpcc::rtos::Scheduler::schedule();	
	
	return 0;
}
