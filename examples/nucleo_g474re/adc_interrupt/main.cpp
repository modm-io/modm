/*
 * Copyright (c) 2020, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/platform/adc/adc_1.hpp>
#include <array>

using namespace modm::literals;

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "STM32G4 ADC interrupt example" << modm::endl;
	MODM_LOG_INFO << " running on Nucleo-G474RE" << modm::endl << modm::endl;
	MODM_LOG_INFO << "GpioA0/Adc1Ch1 and GpioA1/Adc1Ch2 are sampled as an injected sequence." << modm::endl;
	MODM_LOG_INFO << "This sequence is triggered by timer 1 output compare 4 event, which is generated at a freuqnency of 1Hz." << modm::endl;
	MODM_LOG_INFO << "GpioA2/Adc1Ch3 and GpioA3/Adc1Ch4 are sampled as an regular sequence triggered by software." << modm::endl;
	MODM_LOG_INFO << "ADC data is read in an interrupt and printed to this log." << modm::endl;

	MODM_LOG_INFO << "Configuring ADC ...";
	Adc1::initialize(
		Adc1::ClockMode::SynchronousPrescaler1,
		Adc1::ClockSource::SystemClock,
		Adc1::Prescaler::Disabled,
		Adc1::CalibrationMode::SingleEndedInputsMode,
		true);

	GpioA0::setAnalogInput(); // Adc1Ch1
	GpioA1::setAnalogInput(); // Adc1Ch2
	GpioA2::setAnalogInput(); // Adc1Ch3
	GpioA3::setAnalogInput(); // Adc1Ch4

	Adc1::setChannelSampleTime(Adc1::Channel::Channel1, Adc1::SampleTime::Cycles13);
	Adc1::setChannelSampleTime(Adc1::Channel::Channel2, Adc1::SampleTime::Cycles13);
	Adc1::setChannelSampleTime(Adc1::Channel::Channel3, Adc1::SampleTime::Cycles13);
	Adc1::setChannelSampleTime(Adc1::Channel::Channel4, Adc1::SampleTime::Cycles13);

	Adc1::setInjectedChannel(std::array<Adc1::Channel, 2>{Adc1::Channel::Channel1, Adc1::Channel::Channel2});
	Adc1::setChannel(std::array<Adc1::Channel, 2>{Adc1::Channel::Channel3, Adc1::Channel::Channel4});

	Adc1::setInjectedTrigger(Adc1::TriggerMode::RisingEdge, Adc1::TriggerSourceInjected::Tim1Cc4);

	Adc1::enableInterruptVector(5);
	Adc1::enableInterrupt(Adc1::Interrupt::EndOfRegularConversion);
	Adc1::enableInterrupt(Adc1::Interrupt::EndOfInjectedSequenceOfConversions);
	MODM_LOG_INFO << " finished." << modm::endl;

	MODM_LOG_INFO << "Configuring Timer 1 ...";
	Timer1::enable();
	Timer1::setMode(Timer1::Mode::UpCounter);
	// Timer 1 runs at 170_MHz / prescaler=17000 = 10kHz
	Timer1::setPrescaler(17000);
	// Timer over with 1_Hz = 10kHz / overflow=10000
	Timer1::setOverflow(10000);
	Timer1::setCompareValue(4, 1);
	Timer1::start();
	MODM_LOG_INFO << " finished." << modm::endl;

	while (true)
	{
		MODM_LOG_INFO << "Triggering a regular conversion sequence from software." << modm::endl;
		Adc1::startConversion();
		modm::delayMilliseconds(500);
	}

	return 0;
}

MODM_ISR(ADC1)
{
	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfRegularConversion) {
		Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfRegularConversion);
		MODM_LOG_INFO << "Interrupt: regularValue=" << Adc1::getValue() << modm::endl;
	}
	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfInjectedSequenceOfConversions) {
		Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfInjectedSequenceOfConversions);
		MODM_LOG_INFO << "Interrupt: injectedCh1=" << Adc1::getInjectedValue<1>() << modm::endl;
		MODM_LOG_INFO << "Interrupt: injectedCh2=" << Adc1::getInjectedValue<2>() << modm::endl;
	}
}
