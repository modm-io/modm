/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

#include <numbers>
#include <cmath>
#include <array>

using namespace Board;

constexpr auto computeSinTable(float frequency = 1.f)
{
	std::array<uint16_t, 100> data{};
	constexpr auto HalfOutput = ((1 << 12) - 1) / 2; // 12 bit dac
	for (size_t i = 0; i < data.size(); ++i) {
		constexpr auto pi = std::numbers::pi_v<float>;
		data[i] = HalfOutput * (1 + sin(i * (2 * pi * frequency / data.size())));
	}
	return data;
}

constexpr auto sinTable = computeSinTable(1.0f);

// DAC1 channel 1 on GpioA4: constantly output sine signal in circular mode

void setupDac()
{
	using Dac = Dac1Dma;
	using DacChannel = Dac::Channel1<Dma1::Channel1>;

	Dac::connect<GpioOutputA4::Out1>();
	Dac::initialize();

	DacChannel::configure(sinTable.data(), sinTable.size(), DmaBase::CircularMode::Enabled);

	// trigger source 3: timer 4, see reference manual
	DacChannel::setTriggerSource(3);

	DacChannel::startDma();
	DacChannel::enableDacChannel();
}

int main()
{
	Board::initialize();
	LedGreen::setOutput();

	MODM_LOG_INFO << "DAC DMA Demo" << modm::endl;

	Dma1::enable();
	setupDac();

	// configure timer 4 as trigger for DACs
	// 275 MHz / 275 = 1 MHz => 1 Msps DAC output
	Timer4::enable();
	Timer4::setMode(Timer4::Mode::UpCounter);
	Timer4::setPrescaler(1);
	Timer4::setOverflow(275 - 1);
	Timer4::applyAndReset();
	Timer4::start();

	// Enable trigger out for timer 4
	TIM4->CR2 |= TIM_CR2_MMS_1;

	while (true)
	{
		LedGreen::toggle();
		modm::delay_ms(500);
	}

	return 0;
}
