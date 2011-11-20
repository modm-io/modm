
#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

GPIO__OUTPUT(LedStat, C, 12);		// inverted, 0=on, 1=off
GPIO__INPUT(ButtonWakeUp, A, 0);	// 1=pressed, 0=not pressed

GPIO__OUTPUT(Led1, A, 1);

extern "C" void
TIM2_IRQHandler(void)
{
	Timer2::acknowledgeInterrupt(Timer2::UPDATE_INTERRUPT);

	LedStat::toggle();
}

MAIN_FUNCTION
{
	LedStat::setOutput(xpcc::gpio::HIGH);
	Led1::setOutput(xpcc::stm32::ALTERNATE, xpcc::stm32::PUSH_PULL);
	
	// The Button has an external Pull-Down resistor
	ButtonWakeUp::setInput(xpcc::stm32::INPUT, xpcc::stm32::FLOATING);

	Timer2::enable();
	Timer2::configurePwm(Timer2::EDGE_ALIGNED);
	//Timer2::configureCounter();
	Timer2::enableInterrupt(Timer2::UPDATE_INTERRUPT);

	// 72 MHz / 2 / 2^16 ~ 550 Hz
	Timer2::setPrescaler(2);
	Timer2::setOverflow(65535);
	
	//Timer2::configureOutputChannel(2, Timer2::OUTPUT_PWM);
	//Timer2::setCompareChannel(2, 40000);
	TIM2->CCMR1 = 0x6800;
	TIM2->CCMR2 = 0x0000;
	TIM2->CCER  = 0x0010;
	TIM2->CCR2  = 40000;
	
	Timer2::refresh();
	Timer2::resume();

	uint16_t pwm = 0;
	bool up = true;
	while (1)
	{
		if (up) {
			pwm += 10;
			if (pwm >= 65000) {
				up = false;
			}
		}
		else {
			pwm -= 10;
			if (pwm <= 100) {
				up = true;
			}
		}
		TIM2->CCR2 = pwm;
		xpcc::delay_ms(1);
	}
}

