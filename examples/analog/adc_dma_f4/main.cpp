/*
 * Copyright (c) 2023, Zühlke Engineering (Austria) GmbH
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

// This Example uses adc to sample 2 channels in scan mode 50 times every 0.5s and store the value
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
using Adc1Dma = AdcDma<Adc1, Dma2::Channel0>;

std::array<uint16_t, 100> adc_results;
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
	Adc1::initialize<SystemClock>();
	Dma2::enable();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	LedD13::setOutput();
	LedD13::reset();
	advancedTimerConfig<Timer1>(adc_results.size() / 2 - 1);
	Adc1Dma::initialize<SystemClock>((uintptr_t)(&adc_results[0]), adc_results.size());
	const auto c0_channel = Adc1::getPinChannel<GpioInputC0>();
	const auto c1_channel = Adc1::getPinChannel<GpioInputC1>();
	Adc1::connect<GpioInputC0::In10>();
	Adc1::connect<GpioInputC1::In11>();
	Adc1::setChannel(c0_channel, Adc1::SampleTime::Cycles3);
	Adc1::addChannel(c1_channel, Adc1::SampleTime::Cycles3);
	Adc1::enableScanMode();
	// On STM32F4 Event0 means TIM1's channel 1 capture and compare event.
	// Each controller has a different trigger mapping, check the reference
	// manual to more information on the trigger mapping of yours controller.
	Adc1::enableRegularConversionExternalTrigger(Adc1::ExternalTriggerPolarity::FallingEdge,
												 Adc1::RegularConversionExternalTrigger::Event0);
	Adc1Dma::setCompletedConversionCallback(completedCallback);

	adc_results.fill(0);
	timerStart<Timer1>();
	while (true)
	{
		modm::delay(0.5s);
		if (!dma_completed) { continue; }
		dma_completed = false;
		MODM_LOG_DEBUG << "Measurements"
					   << "\r" << modm::endl;
		for (const uint16_t& sample : adc_results) { MODM_LOG_DEBUG << sample << ", "; }
		MODM_LOG_DEBUG << "\r" << modm::endl;
		adc_results.fill(0);
		timerStart<Timer1>();
	}

	return 0;
}
