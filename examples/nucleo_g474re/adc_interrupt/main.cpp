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
#define	MODM_LOG_LEVEL modm::log::DEBUG

int
main()
{
	using namespace modm::platform;

	Board::initialize();

	MODM_LOG_INFO << "STM32G4 ADC interrupt example" << modm::endl;
	MODM_LOG_INFO << " running on Nucleo-G474RE" << modm::endl << modm::endl;
	MODM_LOG_INFO << "GpioA0/Adc1Ch1 and GpioA1/Adc1Ch2 are sampled as an injected sequence." << modm::endl;
	MODM_LOG_INFO << "This sequence is triggered by timer 1 output compare 4 event, which is generated at a freuqnency of 1Hz." << modm::endl;
	MODM_LOG_INFO << "GpioA2/Adc1Ch3 and GpioA3/Adc1Ch4 are sampled as an regular sequence triggered by software." << modm::endl;
	MODM_LOG_INFO << "ADC data is read in an interrupt and printed to this log." << modm::endl;

	MODM_LOG_INFO << "Configuring ADC ...";
	Adc1::initialize<
		Board::SystemClock,
		0_MHz, 0_pct,
		Adc1::ClockMode::SynchronousPrescaler4,
		Adc1::ClockSource::SystemClock>();

	GpioA0::setAnalogInput(); // Adc1Ch1
	GpioA1::setAnalogInput(); // Adc1Ch2
	//GpioA2::setAnalogInput(); // Adc1Ch3
	//GpioA3::setAnalogInput(); // Adc1Ch4

	Adc1::setChannelSampleTime(Adc1::Channel::Channel1, Adc1::SampleTime::Cycles641);
	Adc1::setChannelSampleTime(Adc1::Channel::Channel2, Adc1::SampleTime::Cycles641);
	Adc1::setChannelSampleTime(Adc1::Channel::Channel3, Adc1::SampleTime::Cycles641);
	Adc1::setChannelSampleTime(Adc1::Channel::Channel4, Adc1::SampleTime::Cycles641);

	//Adc1::setInjectedChannel(std::array<Adc1::Channel, 2>{Adc1::Channel::Channel1, Adc1::Channel::Channel2});
	Adc1::setChannel(std::array<Adc1::Channel, 2>{Adc1::Channel::Channel1, Adc1::Channel::Channel2});

	//Adc1::setInjectedTrigger(Adc1::TriggerMode::BothEdges, Adc1::TriggerSourceInjected::Tim1Trgo2);

	Adc1::enableInterruptVector(2);
	Adc1::enableInterrupt(Adc1::Interrupt::EndOfSampling);
	// Adc1::enableInterrupt(Adc1::Interrupt::EndOfRegularConversion);
	// Adc1::enableInterrupt(Adc1::Interrupt::EndOfRegularSequenceOfConversions);
	// Adc1::enableInterrupt(Adc1::Interrupt::EndOfInjectedConversion);
	// Adc1::enableInterrupt(Adc1::Interrupt::EndOfInjectedSequenceOfConversions);
	MODM_LOG_INFO << " finished." << modm::endl;

	/*
	MODM_LOG_INFO << "Configuring Timer 1 ...";
	Timer1::enable();
	Timer1::setMode(Timer1::Mode::UpCounter);

	Timer1::setPeriod<Board::SystemClock>(1'500'000);
	Timer1::enableInterruptVector(Timer1::Interrupt::Update, true, 10);
	Timer1::enableInterrupt(Timer1::Interrupt::Update);

	// Trigger output for ADC
	TIM1->CR2 |= (0b1100 << TIM_CR2_MMS2_Pos);
	
	Timer1::applyAndReset();
	Timer1::start();
	//Timer1::enableOutput();
	*/

	MODM_LOG_INFO << " finished." << modm::endl;

	modm::delay_ms(500);

	// Adc1::connect<GpioA0::In1>();
	// Adc1::setPinChannel<GpioA0>(Adc1::SampleTime::Cycles13);

	Board::LedD13::setOutput();
	Board::LedD13::reset();


	MODM_LOG_INFO << "Starting while(true) loop." << modm::endl;
	while (true)
	{
		MODM_LOG_INFO << "Triggering a regular conversion sequence from software." << modm::endl;
		Board::LedD13::reset();
		modm::delay_ms(1000);
		Adc1::startConversion();
		//Adc1::startInjectedConversion();
		modm::delay_ms(1000);
		// MODM_LOG_INFO << "ADC1->SQR1=" << modm::bin << static_cast<uint32_t>(ADC1->SQR1) << modm::ascii << "\n";
		// MODM_LOG_INFO << "ADC1->JSQR=" << modm::bin << static_cast<uint32_t>(ADC1->JSQR) << modm::ascii << "\n";
	}

	return 0;
}

MODM_ISR(ADC1_2)
{
	Board::LedD13::set();

	modm::delay_ms(1);

	MODM_LOG_DEBUG << "Entry Adc1::getInterruptFlags() = " << modm::bin << Adc1::getInterruptFlags() << modm::ascii << "\n";
	//MODM_LOG_DEBUG << "Entry Adc2::getInterruptFlags() = " << modm::bin << Adc2::getInterruptFlags() << modm::ascii << "\n";

	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfSampling) {
		MODM_LOG_DEBUG << "ADC: EndOfSampling\n";
		Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfSampling);
	}
	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfRegularConversion) {
		MODM_LOG_DEBUG << "ADC: EndOfRegularConversion\n";
		Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfRegularConversion);
		//MODM_LOG_DEBUG << "Interrupt: regularValue=" << Adc1::getValue() << "\n";
	}
	if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfRegularSequenceOfConversions) {
		MODM_LOG_DEBUG << "ADC: EndOfRegularSequenceOfConversions\n";
		Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfRegularSequenceOfConversions);
		//MODM_LOG_DEBUG << "Interrupt: regularValue=" << Adc1::getValue() << "\n";
	}
	// if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfInjectedConversion) {
	// 	Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfInjectedConversion);
	// 	MODM_LOG_DEBUG << "ADC: EndOfInjectedConversion\n";
	// }
	// if (Adc1::getInterruptFlags() & Adc1::InterruptFlag::EndOfInjectedSequenceOfConversions) {
	// 	Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag::EndOfInjectedSequenceOfConversions);
	// 	MODM_LOG_DEBUG << "ADC: EndOfInjectedSequenceOfConversions\n";
	// }
	//MODM_LOG_DEBUG << "MODM_ISR(ADC1_2) exit Adc1::getInterruptFlags() = " << modm::bin << Adc1::getInterruptFlags() << modm::ascii << "\n";
	//Adc1::acknowledgeInterruptFlag(Adc1::InterruptFlag_t{0xffff'ffff});
}

MODM_ISR(TIM1_CC)
{
	if(Timer1::getInterruptFlags() & Timer1::InterruptFlag::CaptureCompare4) {
		MODM_LOG_DEBUG << "TIM1_CC interrupt CaptureCompare4 entered!\n";
		Timer1::acknowledgeInterruptFlags(Timer1::InterruptFlag::CaptureCompare4);
	}
	else {
		MODM_LOG_ERROR << "Unknown TIM1_CC interrupt\n";
	}
}

MODM_ISR(TIM1_UP_TIM16)
{
	if(Timer1::getInterruptFlags() & Timer1::InterruptFlag::Update) {
		MODM_LOG_DEBUG << "TIM1_UP_TIM16 interrupt Update entered!\n";
		Timer1::acknowledgeInterruptFlags(Timer1::InterruptFlag::Update);
	}
	else {
		MODM_LOG_ERROR << "Unknown TIM1_UP_TIM16 interrupt\n";
	}
}
