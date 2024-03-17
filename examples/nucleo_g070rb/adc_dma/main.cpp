/*
 * Copyright (c) 2024, Zühlke Engineering (Austria) GmbH
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// This Example uses adc to sample s channels 50 times every 0.5s and store the value
// in a buffer via dma. The adc is set to be triggered by the Timer1 compare event and then to
// sample both channels using scan mode. The compare event is triggered for a defined number of
// times by configuring the Timer1 to use one pulse mode with an repetition count. For more
// information on the timer please refer to the timer register count example. In this configuration,
// the adc sampling process is started by the cpu but handled completely by peripherals clearing CPU
// time for other tasks.

#include <modm/architecture/interface/interrupt.hpp>
#include <modm/board.hpp>
#include <modm/platform.hpp>

#include "adc_dma.hpp"
#include "timer_handler.hpp"

using namespace Board;
using namespace modm::platform;
using Adc1Dma = AdcDma<Adc1, Dma1::Channel1>;

std::array<uint16_t, 100> adc_results{};
volatile bool dma_completed = false;

void
completedCallback()
{
	LedD13::toggle();
	dma_completed = true;
}

int
main()
{
	Board::initialize();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_INFO << "Start Setup" << modm::endl;

	LedD13::setOutput();
	LedD13::reset();

	Adc1::connect<GpioInputA0::In0>();
	Adc1::connect<GpioInputA1::In1>();

	Adc1::setSampleTime(Adc1::SampleTime::Cycles3_5);
	Adc1::initialize<SystemClock, Adc1::ClockMode::Asynchronous>();
	modm::delay(500ms);
	// On STM32G0 Event1 means TIM1's channel 4 capture and compare event.
	// Each controller has a different trigger mapping, check the reference
	// manual for more information on the trigger mapping of your controller.
	Adc1::enableRegularConversionExternalTrigger(Adc1::ExternalTriggerPolarity::RisingEdge,
												 Adc1::RegularConversionExternalTrigger::Event1);

	Adc1::setChannels(Adc1::channelSequenceFromPins<GpioInputA0, GpioInputA1>());
	Dma1::enable();
	Adc1Dma::initialize<SystemClock>(adc_results);
	Adc1Dma::setCompletedConversionCallback(completedCallback);
	Adc1Dma::startDma();
	Adc1::startConversion();

	advancedTimerConfig<Timer1>(adc_results.size() / 2 - 1);
	timerStart<Timer1>();

	while (true)
	{
		modm::delay(0.5s);
		if (!dma_completed) { continue; }
		dma_completed = false;
		MODM_LOG_INFO << "Measurements"
					  << "\r" << modm::endl;
		for (uint16_t sample : adc_results) { MODM_LOG_INFO << sample << ", "; }
		MODM_LOG_INFO << "\r" << modm::endl;
		adc_results.fill(0);
		timerStart<Timer1>();
	}
	return 0;
}
