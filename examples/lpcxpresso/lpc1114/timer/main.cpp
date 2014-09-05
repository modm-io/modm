
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Led, 0, 7);

typedef xpcc::lpc::Timer16_0 myTimer;

int
main(void)
{
	SystemInit();
	
	// Set LED port pin to output
	Led::setOutput();
	
	myTimer::enable();
	myTimer::setModePwm(0x03, 1000);
	myTimer::setPrescaler(0);

	while (1)
	{
		xpcc::delayMilliseconds(100);
		static uint16_t pwm = 0;
		myTimer::setMatchValue(0, pwm);
		pwm++;
	}

	while (1)
	{

		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}
