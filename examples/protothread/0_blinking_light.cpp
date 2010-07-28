
#include <avr/interrupt.h>
#include <xpcc/architecture/general/gpio.hpp>
#include <xpcc/workflow/protothread.hpp>
#include <xpcc/workflow/timeout.hpp>

GPIO__OUTPUT(LED, B, 0);

class BlinkingLight : public xpcc::ProtothreadLight
{
public:
	bool
	run()
	{
		PT_BEGIN();
		
		// set everything up
		LED::setOutput();
		LED::set();
		
		while (true)
		{
			this->timer.restart(100);
			LED::set();
			PT_WAIT_UNTIL(this->timer.isExpired());
			
			this->timer.restart(200);
			LED::reset();
			PT_WAIT_UNTIL(this->timer.isExpired());
		}
		
		PT_END();
	}

private:
	xpcc::Timeout<> timer;
};

// timer interrupt routine
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
}

MAIN_FUNCTION
{
	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;
	
	// enable interrupts
	sei();
	
	BlinkingLight light;
	while (1)
	{
		light.run();
	}
}
