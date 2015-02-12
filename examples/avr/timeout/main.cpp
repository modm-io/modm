
#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing/timeout.hpp>

using namespace xpcc::atmega;

// create a output device for the led
typedef GpioOutputB0 Led;

// timer interrupt routine
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
}

int
main()
{
	Led::setOutput();
	Led::reset();
	
	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;
	
	// enable interrupts
	enableInterrupts();
	
	xpcc::ShortTimeout timeout(200);
	while (1)
	{
		if (timeout.isExpired())
		{
			timeout.restart(200);
			Led::toggle();
		}
	}
}
