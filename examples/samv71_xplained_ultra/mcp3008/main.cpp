/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/adc/mcp3008.hpp>
#include <utility>

using namespace Board;
using namespace modm::platform;

using SpiMaster = SpiMaster0;
using Sck = GpioD22;
using Mosi = GpioD21;
using Miso = GpioD20;
using Cs = GpioD25;

int main()
{
	Board::initialize();

	MODM_LOG_INFO << "MCP3004/8 ADC example" << modm::endl;

	SpiMaster::connect<Sck::Spck, Mosi::Mosi, Miso::Miso>();
	Cs::setOutput(true);

	SpiMaster::initialize<SystemClock, 1500_kHz, 10_pct>();

	modm::Mcp3008<SpiMaster, Cs> adc;
	adc.initialize();

	constexpr std::array channels = {
		std::make_pair(0, modm::mcp3008::Channel::Ch0),
		std::make_pair(1, modm::mcp3008::Channel::Ch1),
		std::make_pair(2, modm::mcp3008::Channel::Ch2),
		std::make_pair(3, modm::mcp3008::Channel::Ch3)
	};

	while (true) {
		for (auto [i, ch] : channels) {
			const auto value = RF_CALL_BLOCKING(adc.read(ch));
			MODM_LOG_INFO << "channel " << i << ": " << value << '\n';
		}

		Led0::toggle();
		Led1::toggle();
		modm::delay(1s);
	}

	return 0;
}
