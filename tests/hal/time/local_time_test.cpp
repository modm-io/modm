
#include <xpcc/hal/time/local_time.hpp>

using namespace xpcc;

LocalTime::Time xpcc__local_time;

LocalTime starttime(60000);
LocalTime endtime(200);

volatile uint8_t out;

int
main(void)
{
	//LocalTime time = LocalTime::getTime();
	
	if (endtime > starttime) {
		out = 1;
	}

	while (1) {
		
	}
}

