#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#include "../3Motor.hpp"

using namespace xpcc::stm32;


// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG


// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::Low);

	// initialize Uart1 for XPCC_LOG_
	DBG_UART3_TX::connect(Usart3::Tx);
	DBG_UART3_RX::connect(Usart3::Rx, Gpio::InputType::PullUp);
	Usart3::initialize<defaultSystemClock, 115200>(12);

	XPCC_LOG_DEBUG <<"3Motor encoder test" << xpcc::endl;

	xpcc::delayMilliseconds(100);


	ENC1A_T1_1::connect(Timer1::Channel1);
	ENC1B_T1_2::connect(Timer1::Channel2);

	Timer1::enable();
	Timer1::setMode( Timer1::Mode::UpCounter, Timer1::SlaveMode::Encoder3);
	Timer1::setOverflow(0xffff);
	Timer1::applyAndReset();
	Timer1::start();

	ENC2A_T2_1::connect(Timer2::Channel1);
	ENC2B_T2_2::connect(Timer2::Channel2);

	Timer2::enable();
	Timer2::setMode( Timer2::Mode::UpCounter, Timer2::SlaveMode::Encoder3);
	Timer2::setOverflow(0xffff);
	Timer2::applyAndReset();
	Timer2::start();

	ENC3A_T3_1::connect(Timer3::Channel1);
	ENC3B_T3_2::connect(Timer3::Channel2);

	Timer3::enable();
	Timer3::setMode( Timer3::Mode::UpCounter, Timer3::SlaveMode::Encoder3);
	Timer3::setOverflow(0xffff);
	Timer3::applyAndReset();
	Timer3::start();

	ENC4A_T4_1::connect(Timer4::Channel1);
	ENC4B_T4_2::connect(Timer4::Channel2);

	Timer4::enable();
	Timer4::setMode( Timer4::Mode::UpCounter, Timer4::SlaveMode::Encoder3);
	Timer4::setOverflow(0xffff);
	Timer4::applyAndReset();
	Timer4::start();

	ENC8A_T8_1::connect(Timer8::Channel1);
	ENC8B_T8_2::connect(Timer8::Channel2);

	Timer8::enable();
	Timer8::setMode( Timer8::Mode::UpCounter, Timer8::SlaveMode::Encoder3);
	Timer8::setOverflow(0xffff);
	Timer8::applyAndReset();
	Timer8::start();

	while (1)
	{
		LedRed::toggle();
		LedBlue::toggle();

		xpcc::delayMilliseconds(100);

		XPCC_LOG_DEBUG 	<< " ENC1=" << Timer1::getValue()
						<< " ENC2=" << Timer2::getValue()
						<< " ENC3=" << Timer3::getValue()
						<< " ENC4=" << Timer4::getValue()
						<< " ENC8=" << Timer8::getValue() << xpcc::endl;
	}

	return 0;
}
