#include <xpcc/architecture/platform.hpp>
#include <xpcc/processing.hpp>
#include "../stm32f4_discovery.hpp"
#include "leds.hpp"

// create the leds
xpcc::ui::Led orange(setOrange);
xpcc::ui::Led red(setRed);
xpcc::ui::Led green(setGreen);
xpcc::ui::Led blue(setBlue);

// apply some animations to the leds
xpcc::ui::Pulse<uint8_t> pulse(red.animation);
xpcc::ui::Indicator<uint8_t> indicator(blue.animation);
xpcc::ui::Strobe<uint8_t> strobe(green.animation);

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	// connect the Timer Channels to the LEDs
	LedGreen::connect(Timer4::Channel1);
	LedOrange::connect(Timer4::Channel2);
	LedRed::connect(Timer4::Channel3);
	LedBlue::connect(Timer4::Channel4);

	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);

	// 42 MHz / 1 / 2^16 ~ 640 Hz refresh rate
	Timer4::setPrescaler(1);
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

	// set these animations to expire after x loops
	pulse.start();
	indicator.start();
	strobe.start();
	// fade this led for 32s
	orange.fadeTo(255, 32000);

	while (1)
	{
		// update all
		pulse.update();
		indicator.update();
		strobe.update();

		blue.update();
		red.update();
		orange.update();
		green.update();
	}

	return 0;
}
