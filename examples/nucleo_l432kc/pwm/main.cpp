#include <xpcc/architecture/platform.hpp>

using namespace Board;

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::INFO

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	XPCC_LOG_INFO << "Nucleo L432KC PWM example" << xpcc::endl;

	LedGreen::set();

	uint32_t counter = 0;

	GpioOutputA8::connect(Timer1::Channel1);
	GpioOutputA9::connect(Timer1::Channel2);
	GpioOutputA10::connect(Timer1::Channel3);
	GpioOutputA11::connect(Timer1::Channel4);

	Timer1::enable();
	Timer1::setMode(Timer1::Mode::UpCounter);

	// 80 MHz / 800 / 2^16 ~ 1.5 Hz
	Timer1::setPrescaler(800);
	Timer1::setOverflow(65535);


	Timer1::configureOutputChannel(1, Timer2::OutputCompareMode::Pwm, 32767);
	Timer1::configureOutputChannel(2, Timer2::OutputCompareMode::Pwm, 32767);
	Timer1::configureOutputChannel(3, Timer2::OutputCompareMode::Pwm, 32767);
	Timer1::configureOutputChannel(4, Timer2::OutputCompareMode::Pwm, 32767);

	Timer1::applyAndReset();

	Timer1::start();
	Timer1::enableOutput();

	while (true)
	{
		LedGreen::toggle();
		xpcc::delayMilliseconds(500);
		XPCC_LOG_INFO << "loop: " << counter++ << xpcc::endl;
		XPCC_LOG_INFO << "Timer1 counter: " << Timer1::getValue() << xpcc::endl;
	}

	return 0;
}
