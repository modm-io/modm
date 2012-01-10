
#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

GPIO__OUTPUT(Led1, A, 1);
GPIO__OUTPUT(Led2, A, 8);

static bool
initClock()
{
	typedef xpcc::stm32::Clock C;
	
	// use external 8MHz crystal, stm32f1
	if (!C::enableHse(C::HSE_CRYSTAL)) {
		return false;
	}
	
	C::enablePll(C::PLL_HSE, C::PLL_MUL_9);
	return C::switchToPll();
}

MAIN_FUNCTION
{
	initClock();

	Led1::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	Led2::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	
	// Set up Timer 2 (for LED1 connected to PA1)
	Timer2::enable();
	Timer2::setMode(Timer2::UP_COUNTER);

	// 72 MHz / 2 / 2^16 ~ 550 Hz
	Timer2::setPrescaler(2);
	Timer2::setOverflow(65535);

	Timer2::configureOutputChannel(2, Timer2::OUTPUT_PWM, 40000);
	Timer2::applyAndReset();

	Timer2::start();

	// Set up Timer 1 (for LED2 connected to PA8)
	Timer1::enable();
	Timer1::setMode(Timer1::UP_COUNTER);

	// 72 MHz / 2 / 2^16 ~ 550 Hz
	Timer1::setPrescaler(2);
	Timer1::setOverflow(65535);
	Timer1::enableOutput();

	Timer1::configureOutputChannel(1, Timer1::OUTPUT_PWM, 40000);
	Timer1::applyAndReset();
	
	Timer1::start();

	uint16_t pwm = 0;
	bool up = true;
	while (1)
	{
		// Let the LED fade up and down
		if (up) {
			pwm += 1;
			if (pwm >= 255) {
				up = false;
			}
		}
		else {
			pwm -= 1;
			if (pwm <= 0) {
				up = true;
			}
		}
		Timer1::setCompareValue(1, pwm * pwm);
		Timer2::setCompareValue(2, (255 - pwm) * (255 - pwm));

		xpcc::delay_ms(5);
	}
}

