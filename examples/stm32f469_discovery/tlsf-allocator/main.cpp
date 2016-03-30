#include <xpcc/architecture/platform.hpp>
#include <tlsf.h>

extern tlsf_t tlsf_heap0;
extern uint32_t __heap1_start;

int main()
{
	Board::initialize();
	Board::Leds::setOutput();

	// 200 pointers to allocated memories
	void *d[200];
	int free_ii = 0;

	for (int ii=0; ii < 200; ii++)
	{
		d[ii] = nullptr;
		uint32_t size = rand() % (1024*20);

		if ((rand() % 100) >= 75) {
			d[ii] = tlsf_malloc(tlsf_heap0, size);
			// explicit allocation in CCM
			XPCC_LOG_INFO << ".heap0  ";
		} else {
			d[ii] = malloc(size);
			if (d[ii] && d[ii] < &__heap1_start) {
				// allocation on SRAM has failed and fallback uses CCM!
				XPCC_LOG_INFO << ".heap0FB";
			}
			else XPCC_LOG_INFO << ".heap123";
		}

		// print what size we requested and if it succeeded
		XPCC_LOG_INFO.printf(" malloc(%2ukB) = ", size/1024);
		if (d[ii]) XPCC_LOG_INFO << d[ii];
		else   XPCC_LOG_INFO << "NO MEMORY ";

		if ((rand() % 100) >= 50) {
			// only some memory is returned to the system
			void* df = d[free_ii++];
			XPCC_LOG_INFO << "  ...freeing " << df;
			free(df);
		}
		XPCC_LOG_INFO << xpcc::endl;
	}

	while (1)
	{
		Board::Leds::toggle();
		xpcc::delayMilliseconds(500);
	}
	return 0;
}
