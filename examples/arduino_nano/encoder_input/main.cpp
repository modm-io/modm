/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/encoder/bitbang_encoder_input.hpp>
#include <modm/math/algorithm/prescaler.hpp>
#include <modm/processing/timer.hpp>

using namespace modm::platform;

// Connect the encoders outputs to D7 and D8 Pins (usually the outer pins)
// The common third pin (usually in the middle) is connected to GND.
// Don't add any resistors or filters. It's all in the MCU and the driver.
modm::BitBangEncoderInput<Board::D11, Board::D12, 4> encoder;

MODM_ISR(TIMER2_COMPA)
{
	encoder.update();
}

void
init_Timer2()
{
	constexpr float f = 1_kHz;
	constexpr auto prescaler =
		modm::Prescaler::from_list(SystemClock::Timer, f * 2, {0, 1, 8, 32, 64, 128, 256, 1024});
	constexpr auto ocr = (prescaler.frequency / f) - 1;
	static_assert(ocr <= 255, "Can't configure Timer2 for desired f");

	// Timer in CTC Mode
	TCCR2A = (1 << WGM21);
	OCR2A = ocr;
	TIMSK2 |= (1 << OCIE2A);
	TCCR2B |= prescaler.index;
}

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	encoder.connect();

	init_Timer2();
	enableInterrupts();

	int value(0);

	modm::ShortPeriodicTimer heartbeat(500ms);
	modm::ShortPeriodicTimer outputValue(1000ms);

	while (true)
	{
		if (heartbeat.execute()) Board::LedD13::toggle();
		if (outputValue.execute())
		{
			value += encoder.getIncrement();
			MODM_LOG_INFO << "value: " << value << modm::endl;
		}
	}
}
