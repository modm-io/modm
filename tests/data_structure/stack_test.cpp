
#include <xpcc/data_structure/stack.hpp>

using namespace xpcc;

BoundedDeque<uint16_t, 300> deque;

volatile uint16_t a;

int
main(void)
{
	deque.pushBack(14);
	
	a = deque.getSize();
	
	while (1) {
		
	}
}
