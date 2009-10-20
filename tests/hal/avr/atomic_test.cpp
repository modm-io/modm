
#include <xpcc/hal/atomic.hpp>

using namespace xpcc;

atomic::Container<uint32_t> container;

//atomic::Flag flag(true);

volatile uint8_t out;

int main(void)
{
	/*flag.reset();
	if (flag.isSet()) {
		PORTB = 1;
	}*/
	
	uint32_t value = 12;
	container.set(value);
	
	container.getObject()++;
	
	container.getObject() = 26;
	
	if (container.get()) {
		out = 1;
	}
	
	while (1) {
		
	}
}
