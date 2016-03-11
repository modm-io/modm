#include <xpcc/architecture/platform.hpp>

int
main()
{
	Board::initialize();
	Board::LedNorth::set();


	Timer1::enable();
	//Timer1::setMode(Timer1::Mode::CenterAligned3);
	Timer1::setMode(Timer1::Mode::UpCounter);
	Timer1::setOverflow(0xff);
	//Timer1::setPeriod(4);
	Timer1::applyAndReset();
	Timer1::start();

	Board::LedNorth::connect(Timer1::Channel1);
	Board::LedNorthWest::connect(Timer1::Channel1N);
	Timer1::setCompareValue(1, 0);
	Timer1::configureOutputChannel(1,
			static_cast<uint32_t>(Timer1::OutputCompareMode::Pwm) | 0b0101);
	Timer1::setDeadTime(7);

	Timer1::enableOutput();

	uint8_t i = 0;
	while (1){
		Timer1::setCompareValue(1, ++i);
		xpcc::delayMilliseconds(10);
	}

	return 0;
}
