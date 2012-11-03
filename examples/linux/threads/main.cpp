
#include <xpcc/architecture.hpp>
#include <xpcc/processing/rtos.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
xpcc::rtos::BinarySemaphore event;

// ----------------------------------------------------------------------------
class Thread1 : public xpcc::rtos::Thread
{
public:
	Thread1() :
		xpcc::rtos::Thread(2)
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

class Thread2 : public xpcc::rtos::Thread
{
public:
	Thread2() :
		xpcc::rtos::Thread(2)
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
Thread1 thread1;
Thread2 thread2;

int
main(void)
{
	// start scheduler
	xpcc::rtos::Scheduler::schedule();	
	
	return 0;
}
