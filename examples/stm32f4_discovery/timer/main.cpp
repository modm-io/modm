
#include <xpcc/architecture.hpp>
#include <xpcc/workflow.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedOrange, D, 13);		// User LED 3
GPIO__OUTPUT(LedGreen, D, 12);		// User LED 4
GPIO__OUTPUT(LedRed, D, 14);		// User LED 5
GPIO__OUTPUT(LedBlue, D, 15);		// User LED 6

GPIO__OUTPUT(VBusPresent, A, 9);		// green LED (LD7)
GPIO__OUTPUT(VBusOvercurrent, D, 5);	// red LED   (LD8)

GPIO__INPUT(Button, A, 0);

using namespace xpcc::stm32;

static bool
initClock()
{
	// use external 8MHz crystal
	if (!Clock::enableHse(Clock::HseConfig::HSE_CRYSTAL)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, 4, 168);
	return Clock::switchToPll();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedOrange::setAlternateFunction(AF_TIM4, PUSH_PULL);
	LedGreen::setAlternateFunction(AF_TIM4, PUSH_PULL);
	LedRed::setAlternateFunction(AF_TIM4, PUSH_PULL);
	LedBlue::setAlternateFunction(AF_TIM4, PUSH_PULL);
		
	Timer4::enable();
	Timer4::setMode(Timer4::UP_COUNTER);
	
	// 42 MHz / 2 / 2^16 ~ 320 Hz
	Timer4::setPrescaler(2);
	Timer4::setOverflow(65535);
	
	Timer4::configureOutputChannel(1, Timer2::OUTPUT_PWM, 32768);
	Timer4::configureOutputChannel(2, Timer2::OUTPUT_PWM, 32768);
	Timer4::configureOutputChannel(3, Timer2::OUTPUT_PWM, 32768);
	Timer4::configureOutputChannel(4, Timer2::OUTPUT_PWM, 32768);
	Timer4::applyAndReset();
	
	Timer4::start();
	
	// Start the systick timer to generate a 1ms systemclock
	SysTickTimer::enable();
	
	uint16_t pwm[4] = { 0, 64, 128, 192 };
	bool up[4] = { true, true, true, true };
	xpcc::PeriodicTimer<> timer(5);
	while (1)
	{
		if (timer.isExpired())
		{
			// Let the LEDs fade up and down
			for (std::size_t i = 0; i < 4; ++i) {
				if (up[i]) {
					pwm[i] += 1;
					if (pwm[i] >= 255) {
						up[i] = false;
					}
				}
				else {
					pwm[i] -= 1;
					if (pwm[i] <= 0) {
						up[i] = true;
					}
				}
			}
			
			Timer4::setCompareValue(1, pwm[0] * pwm[0]);
			Timer4::setCompareValue(2, pwm[1] * pwm[1]);
			Timer4::setCompareValue(3, pwm[2] * pwm[2]);
			Timer4::setCompareValue(4, pwm[3] * pwm[3]);
		}
	}

	return 0;
}
