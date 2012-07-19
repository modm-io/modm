
// this header will include the right header for our architecture
#include <xpcc/architecture.hpp>

// create a wrapper for a output pin
GPIO__OUTPUT(Led, A, 12);

int
main(void)
{
	Led::setOutput();

	while (1)
	{
		Led::reset();
		for (uint32_t ii = 0; ii < 8192; ++ii)
		{
			asm volatile ("nop");
		}
		Led::set();
		for (uint32_t ii = 0; ii < 16384; ++ii)
		{
			asm volatile ("nop");
		}
	}

	while (1)
	{
	}
}
