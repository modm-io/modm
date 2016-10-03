#ifndef THREAD_BLINK
#define THREAD_BLINK

#include <xpcc/processing/timer.hpp>
#include <xpcc/processing/protothread.hpp>

class BlinkThread: public xpcc::pt::Protothread
{
public:
	BlinkThread();

	bool
	update();

private:
	xpcc::ShortTimeout timeout;
	xpcc::ShortPeriodicTimer timer;
	uint32_t uptime;
};

#endif // THREAD_BLINK
