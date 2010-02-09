
#include <stdint.h>
#include <xpcc/math/filter/pid.hpp>

using namespace xpcc;

Pid<int16_t, 64>::Parameter parameter(10, 1, 0, 100, 120);
Pid<int16_t, 64> pid(parameter);

volatile int16_t out;

int main(void)
{
	pid.reset();
	pid.setTarget(100);

	pid.update(10);
	pid.update(20);
	pid.update(30);

	out = pid.getValue();

	while(1) {
		
	}
}
