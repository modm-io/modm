#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing.hpp>
#include "../stm32f4_discovery.hpp"

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::connect(Timer4::Channel2);
	LedGreen::connect(Timer4::Channel1);
	LedRed::connect(Timer4::Channel3);
	LedBlue::connect(Timer4::Channel4);

	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);

	// 42 MHz / 2 / 2^16 ~ 320 Hz
	Timer4::setPrescaler(2);
	Timer4::setOverflow(65535);

	Timer4::configureOutputChannel(1, Timer2::OutputCompareMode::Pwm, 32768);
	Timer4::configureOutputChannel(2, Timer2::OutputCompareMode::Pwm, 32768);
	Timer4::configureOutputChannel(3, Timer2::OutputCompareMode::Pwm, 32768);
	Timer4::configureOutputChannel(4, Timer2::OutputCompareMode::Pwm, 32768);
	Timer4::applyAndReset();

	Timer4::start();

	// Start the systick timer to generate a 1ms systemclock
	// this is needed for the xpcc::PeriodicTimer to function
	xpcc::cortex::SysTickTimer::enable();

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
