/*
 * Copyright (c) 2025, Henrik Hose
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

// Tested with MIKROE LIGHTRANGER 12 CLICK Breakout
// with VL53L8CH variant, so histogram example 12 is supported
extern "C" {
#include "../../../../ext/vl53/vl53/vl53lmz/examples/Example_12_cnh_data.h"
#include "../../../../ext/vl53/vl53/vl53lmz/examples/Example_1_ranging_basic.h"
#include "../../../../ext/vl53/vl53/vl53lmz/examples/Example_2_get_set_params.h"
#include "../../../../ext/vl53/vl53/vl53lmz/examples/Example_3_ranging_modes.h"
#include "../../../../ext/vl53/vl53/vl53lmz/examples/Example_5_multiple_targets_per_zone.h"
}

#include <modm/driver/vl53/vl53_transport.hpp>

using namespace Board;

using SpiMaster = SpiMaster2_Dma<Dma1::Channel0, Dma1::Channel1>;
using Cs = GpioC0;
using Mosi = GpioB15;
using Miso = GpioC2;
using Sck = GpioD3;

using Rst = GpioD6;
using Snc = GpioB1;
using LPn = GpioA3;
using Int = GpioD7;

modm::Vl53SpiTransport<SpiMaster, Cs, LPn> transport;

int
main()
{
	Board::initialize();

	Dma1::enable();
	Leds::setOutput();

	LPn::setOutput(false);
	Snc::setOutput(false);
	Cs::setOutput(true);

	transport.resetSensor();

	SpiMaster::connect<Sck::Sck, Mosi::Mosi, Miso::Miso>();
	SpiMaster::initialize<Board::SystemClock, 2.2_MHz>();

	example1(&transport);
	example2(&transport);
	example3(&transport);
	example5(&transport);
	example12(&transport);
}
