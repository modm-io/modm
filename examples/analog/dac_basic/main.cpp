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

constexpr auto computeSinTable()
{
	std::array<uint16_t, 100> data{};
	constexpr auto HalfOutput = ((1 << 12) - 1) / 2; // 12 bit dac
	for (size_t i = 0; i < data.size(); ++i) {
		constexpr auto pi = std::numbers::pi_v<float>;
		data[i] = HalfOutput * (1 + sin(i * (2*pi / data.size())));
	}
	return data;
}

constexpr auto sinTable = computeSinTable();

int
main()
{
	Board::initialize();
	LedBlue::setOutput();

	MODM_LOG_INFO << "DAC Demo" << modm::endl;

	// Output ~8 kHz sine / cosine waveforms on gpio A4/A5
	Dac::connect<GpioA4::Out1, GpioA5::Out2>();
	Dac::initialize();

	Dac::enableChannel(Dac::Channel::Channel1);
	Dac::enableChannel(Dac::Channel::Channel2);
	Dac::enableOutputBuffer(Dac::Channel::Channel1, true);
	Dac::enableOutputBuffer(Dac::Channel::Channel2, true);

	uint32_t counter = 0;
	while (true)
	{
		constexpr auto size = sinTable.size();
		const auto sinIndex = counter % size;
		const auto cosIndex = (counter + (size/4)) % size;
		counter++;

		Dac::setOutput1(sinTable[sinIndex]);
		Dac::setOutput2(sinTable[cosIndex]);

		LedBlue::toggle();
		modm::delay_us(1);
	}

	return 0;
}
