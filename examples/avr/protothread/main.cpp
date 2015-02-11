
#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing/protothread.hpp>
#include <xpcc/processing/timeout.hpp>

using namespace xpcc::atmega;

typedef GpioOutputB0 LedGreen;
typedef GpioOutputB1 LedRed;

class BlinkingLightGreen : public xpcc::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();
		
		// set everything up
		LedGreen::setOutput();
		LedGreen::set();
		
		while (true)
		{
			LedGreen::set();
			
			this->timer.restart(100);
			PT_WAIT_UNTIL(this->timer.isExpired());
			
			LedGreen::reset();
			
			this->timer.restart(600);
			PT_WAIT_UNTIL(this->timer.isExpired());
		}
		
		PT_END();
	}

private:
	xpcc::ShortTimeout timer;
};

class BlinkingLightRed : public xpcc::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();
		
		// set everything up
		LedRed::setOutput();
		LedRed::set();
		
		while (true)
		{
			LedRed::set();
			
			this->timer.restart(200);
			PT_WAIT_UNTIL(this->timer.isExpired());
			
			LedRed::reset();
			
			this->timer.restart(300);
			PT_WAIT_UNTIL(this->timer.isExpired());
			
			LedRed::set();
			
			this->timer.restart(200);
			PT_WAIT_UNTIL(this->timer.isExpired());
			
			LedRed::reset();
			
			this->timer.restart(1000);
			PT_WAIT_UNTIL(this->timer.isExpired());
		}
		
		PT_END();
	}

private:
	xpcc::ShortTimeout timer;
};

// timer interrupt routine
ISR(TIMER2_COMPA_vect)
{
	xpcc::Clock::increment();
}

int
main()
{
	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;
	
	enableInterrupts();
	
	BlinkingLightGreen greenLight;
	BlinkingLightRed redLight;
	while (1)
	{
		greenLight.run();
		redLight.run();
	}
}
