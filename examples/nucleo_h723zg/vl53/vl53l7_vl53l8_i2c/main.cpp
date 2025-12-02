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

using I2c = I2cMaster1;
using Scl = GpioB8;  // D15
using Sda = GpioB9;  // D14

using Rst = GpioD6;
using Snc = GpioB1;
using LPn = GpioA3;
using Int = GpioD7;

modm::Vl53I2cTransport<I2c, LPn> transport{VL53LMZ_DEFAULT_I2C_ADDRESS >> 1};

int
main()
{
	Board::initialize();

	Leds::setOutput();

	LPn::setOutput(false);
	Snc::setOutput(false);

	transport.resetSensor();

	I2c::connect<Scl::Scl, Sda::Sda>(I2c::PullUps::Internal);
	I2c::initialize<Board::SystemClock, 1_MHz>();

	example1(&transport);
	example2(&transport);
	example3(&transport);
	example5(&transport);
	example12(&transport);
}
