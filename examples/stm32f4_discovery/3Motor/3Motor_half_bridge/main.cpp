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

	XPCC_LOG_DEBUG <<"3Motor half bridge test" << xpcc::endl;

	xpcc::delayMilliseconds(100);

	M1_EN_A::setOutput(xpcc::Gpio::Low);
	M1_EN_B::setOutput(xpcc::Gpio::Low);
	M1_EN_C::setOutput(xpcc::Gpio::Low);
	M1_PWM_A_T10_1::connect(Timer10::Channel1);
	M1_PWM_B_T11_1::connect(Timer11::Channel1);
	M1_PWM_C_T9_1::connect(Timer9::Channel1);
	NOT_M1_OTW::setInput();    // DRV8332 has internal pull-ups to 3.3v
	NOT_M1_FAULT::setInput();  // DRV8332 has internal pull-ups to 3.3v

	M2_EN_A::setOutput(xpcc::Gpio::Low);
	M2_EN_B::setOutput(xpcc::Gpio::Low);
	M2_EN_C::setOutput(xpcc::Gpio::Low);
	M2_PWM_A_T9_2::connect(Timer9::Channel2, Gpio::OutputType::PushPull);
	M2_PWM_B_T5_1::connect(Timer5::Channel1);
	M2_PWM_C_T5_2::connect(Timer5::Channel2);
	NOT_M2_OTW::setInput();    // DRV8332 has internal pull-ups to 3.3v
	NOT_M2_FAULT::setInput();  // DRV8332 has internal pull-ups to 3.3v


	uint32_t offsetOversampling = 128;
	xpcc::stm32::Adc1::initialize( xpcc::stm32::Adc1::Prescaler::Div8 );

	{
		using namespace CurrentSensors;
		ADC123_IN13_M2_A::connect(Adc1::Channel13);
		Adc1::setChannel( Adc1::Channel::Channel13, Adc1::SampleTime::Cycles28);
		uint32_t i;
		current_M2_A_offset = 0;
		for (i=0 ; i<offsetOversampling ; i++)
		{
			Adc1::startConversion();
			while (!xpcc::stm32::Adc1::isConversionFinished());
			current_M2_A_offset += Adc1::getValue();
		}
		current_M2_A_offset /= offsetOversampling;
		XPCC_LOG_DEBUG << "current_M2_A_offset=" << current_M2_A_offset << xpcc::endl;

		ADC123_IN2_M2_B::connect(Adc1::Channel2);
		Adc1::setChannel( Adc1::Channel::Channel2, Adc1::SampleTime::Cycles28);
		current_M2_B_offset = 0;
		for (i=0 ; i<offsetOversampling ; i++)
		{
			Adc1::startConversion();
			while (!xpcc::stm32::Adc1::isConversionFinished());
			current_M2_B_offset += Adc1::getValue();
		}
		current_M2_B_offset /= offsetOversampling;
		XPCC_LOG_DEBUG << "current_M2_B_offset=" << current_M2_B_offset << xpcc::endl;

		ADC123_IN3_M2_C::connect(Adc1::Channel3);
		Adc1::setChannel( Adc1::Channel::Channel3, Adc1::SampleTime::Cycles28);
		current_M2_C_offset = 0;
		for (i=0 ; i<offsetOversampling ; i++)
		{
			Adc1::startConversion();
			while (!xpcc::stm32::Adc1::isConversionFinished());
			current_M2_C_offset += Adc1::getValue();
		}
		current_M2_C_offset /= offsetOversampling;
		XPCC_LOG_DEBUG << "current_M2_C_offset=" << current_M2_C_offset << xpcc::endl;
	}





	Timer10::enable();
	Timer10::setMode(Timer10::Mode::UpCounter);
	Timer10::setPrescaler(4);
	Timer10::setOverflow(1023);
	Timer10::configureOutputChannel(1, Timer10::OutputCompareMode::Pwm, 0);
	Timer10::applyAndReset();
	Timer10::setValue(4);

	Timer11::enable();
	Timer11::setMode(Timer11::Mode::UpCounter);
	Timer11::setPrescaler(4);
	Timer11::setOverflow(1023);
	Timer11::configureOutputChannel(1, Timer11::OutputCompareMode::Pwm, 0);
	Timer11::applyAndReset();
	Timer11::setValue(7);

	Timer9::enable();
	Timer9::setMode(Timer9::Mode::UpCounter);
	Timer9::setPrescaler(4);
	Timer9::setOverflow(1023);
	Timer9::configureOutputChannel(1, Timer9::OutputCompareMode::Pwm, 0);
	Timer9::configureOutputChannel(2, Timer9::OutputCompareMode::Pwm, 0);
	Timer9::applyAndReset();

	Timer5::enable();
	Timer5::setMode(Timer5::Mode::UpCounter);
	Timer5::setPrescaler(2);
	Timer5::setOverflow(1023);
	Timer5::configureOutputChannel(1, Timer5::OutputCompareMode::Pwm, 0);
	Timer5::configureOutputChannel(2, Timer5::OutputCompareMode::Pwm, 0);
	Timer5::applyAndReset();

	Timer5::start();
	Timer9::start();
	Timer10::start();
	Timer11::start();



	M1_EN_A::set();
	M1_EN_B::set();
	M1_EN_C::set();

	M2_EN_A::set();
	M2_EN_B::set();
	M2_EN_C::set();

	float currentFiltered = 0.0;

	while (1)
	{
		LedRed::toggle();
		LedBlue::toggle();

		int i;
		for (i=-1023 ; i<1024 ; i++)
		{
			Timer10::setCompareValue(1, 512); //M1_A
			Timer11::setCompareValue(1, abs(i)); //M1_B
			Timer9::setCompareValue(1, 512);  //M1_C

			Timer9::setCompareValue(2, abs(i));  // M2_A
			Timer5::setCompareValue(1, 512);  // M2_B
			Timer5::setCompareValue(2, abs(i));  // M2_C


			/*
			if (i>0)
			{
				Timer5::setCompareValue(2, 20);
			}else{
				Timer5::setCompareValue(2, 1003);
			}*/


			xpcc::delayMilliseconds(1);


			Adc1::setChannel( Adc1::Channel::Channel2, Adc1::SampleTime::Cycles28);
			Adc1::startConversion();
			while (!xpcc::stm32::Adc1::isConversionFinished());
			int32_t currentRawB = Adc1::getValue();
			currentRawB -= CurrentSensors::current_M2_B_offset;


			Adc1::setChannel( Adc1::Channel::Channel3, Adc1::SampleTime::Cycles28);
			Adc1::startConversion();
			while (!xpcc::stm32::Adc1::isConversionFinished());
			int32_t currentRawC = Adc1::getValue();
			currentRawC -= CurrentSensors::current_M2_C_offset;

			int32_t current = currentRawB - currentRawC;
			currentFiltered = 0.98*currentFiltered + 0.02*current;

		}

		XPCC_LOG_DEBUG <<"NOT_M1_OTW=" << NOT_M1_OTW::read() << "  NOT_M1_FAULT=" << NOT_M1_FAULT::read
					   <<"  NOT_M2_OTW=" << NOT_M2_OTW::read() << "  NOT_M2_FAULT=" << NOT_M2_FAULT::read <<  xpcc::endl;
	}

	return 0;
}
