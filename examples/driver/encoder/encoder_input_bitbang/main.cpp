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
#include <modm/processing/timer.hpp>
#include <modm/driver/encoder/bitbang_encoder_input.hpp>

using namespace modm::platform;

// Connect the outputs of the encoder to these pins (usually the outer pins).
// The common third pin (usually in the middle) is connected to GND.
#ifdef MODM_CPU_AVR
modm::BitBangEncoderInput<Board::D11, Board::D12, 4> encoder;
#else
modm::BitBangEncoderInput<GpioInputB6, GpioInputB7, 4, int16_t> encoder;
#endif

// The encoder is sampled by a 1 kHz timer interrupt
#ifdef MODM_CPU_AVR
#include <modm/math/algorithm/prescaler.hpp>

MODM_ISR(TIMER2_COMPA)
{
	encoder.update();
}

static void
initPeriodicInterrupt()
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
	enableInterrupts();
}
#else
MODM_ISR(TIM2)
{
	Timer2::acknowledgeInterruptFlags(Timer2::InterruptFlag::Update);
	encoder.update();
}

static void
initPeriodicInterrupt()
{
	Timer2::enable();
	Timer2::setMode(Timer2::Mode::UpCounter);
	Timer2::setPeriod<Board::SystemClock>(1ms);
	Timer2::enableInterruptVector(true, 10);
	Timer2::enableInterrupt(Timer2::Interrupt::Update);
	Timer2::applyAndReset();
	Timer2::start();
}
#endif

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	encoder.initialize();
	initPeriodicInterrupt();

	int value(0);
	modm::ShortPeriodicTimer heartbeat(500ms);

	while (true)
	{
		if (heartbeat.execute())
		{
			Board::Leds::toggle();

			const auto delta = encoder.getDelta();
			value += delta;
			MODM_LOG_INFO << "Delta: " << delta << ", absolute: " << value << modm::endl;
		}
	}
}
