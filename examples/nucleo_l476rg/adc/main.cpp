#include <modm/architecture/platform.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	// Output MSI Clock at MCO
	// 48 MHz / 16 = 3 MHz expected at PA8
	ClockControl::enableClockOutput(
		ClockControl::ClockOutputSource::MultiSpeedInternalClock,
		ClockControl::ClockOutputSourcePrescaler::Div16);

	modm::stm32::GpioA8::connect(modm::stm32::ClockControl::ClockOutput);

	LedGreen::set();

	Adc1::initialize(Adc1::ClockMode::SynchronousPrescaler1,
					Adc1::ClockSource::SystemClock,
					Adc1::Prescaler::Disabled,
					Adc1::CalibrationMode::SingleEndedInputsMode, true);
	GpioInputA0::connect(Adc1::Channel5);
	Adc1::setChannel(Adc1::Channel::Channel5, Adc1::SampleTime::Cycles182);

	int loop(0);
	while (true)
	{
		LedGreen::toggle();
		modm::delayMilliseconds(Button::read() ? 250 : 500);

		Adc1::startConversion();
		// wait for conversion to finish
		while(not Adc1::isConversionFinished())
			{};
		// print result
		int adcValue = Adc1::getValue();
		MODM_LOG_INFO.printf("loop=%d: adcValue=%d\n", loop++, adcValue);
	}

	return 0;
}
