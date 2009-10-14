
#include <avr/io.h>

#include "../../../src/hal/time/local_time.hpp"

using namespace xpcc;

LocalTime::Time xpcc__local_time;

LocalTime starttime(60000);
LocalTime endtime(200);

int
main(void)
{
	//LocalTime time = LocalTime::getTime();
	
	if (endtime > starttime) {
		PORTB = 1;
	}

	while (1) {
		
	}
}

