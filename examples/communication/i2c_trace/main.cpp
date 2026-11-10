/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/platform/i2c/bitbang_i2c_master.hpp>
#include "i2c_recorder.hpp"

/*
 * Traces the steps of the I2C controllers with MODM_I2C_TRACE (set in project.xml) into a
 * recorder, and prints them after every transaction: a read with the I2C peripheral, then
 * the same read with the bit-banged controller on the same pins.
 *
 * Connect a device to SCL (D15) and SDA (D14), or nothing: then the reads end with an
 * address NACK, which is not a failure. A failure (a timeout, a lost arbitration, a bus
 * error, ...) freezes the recording of its controller, so that the steps up to it stay
 * until resume().
 */

using namespace Board;
using namespace std::chrono_literals;
using I2c = I2cMaster1;
using BitBang = BitBangI2cMaster<D15, D14>;

static I2cRecorder<32> recorder;

// called by the drivers: from the I2C interrupt or with the interrupts disabled (I2cMaster1),
// from the fiber in start() (BitBangI2cMaster)
void
i2c_trace(uintptr_t instance, uint8_t before, uint8_t flag, uint8_t action, uint8_t after)
{
	recorder.add(instance, {before, flag, action, after});
}

int
main()
{
	Board::initialize();
	I2c::initialize<SystemClock, 100_kHz>();
	BitBang::initialize<SystemClock, 100_kHz>();
	modm::I2cDevice<I2c> device{0x50};
	modm::I2cDevice<BitBang> bitbang{0x50};
	device.setTimeout(10ms);
	bitbang.setTimeout(10ms);
	const uint8_t reg[1] = {0};
	uint8_t data[4];

	while (true)
	{
		I2c::connect<D15::Scl, D14::Sda>(I2c::PullUps::Internal);
		bool ok = device.writeRead(reg, sizeof(reg), data, sizeof(data));
		MODM_LOG_INFO << "I2cMaster1: read " << (ok ? "ok" : "failed") << ", the steps:" << modm::endl;
		auto *ring = recorder.ring(I2C1_BASE);
		printI2cTrace<modm::platform::I2cMasterBase>(modm::log::info, *ring);
		ring->resume();

		BitBang::connect<D15::BitBang, D14::BitBang>(BitBang::PullUps::Internal);
		ok = bitbang.writeRead(reg, sizeof(reg), data, sizeof(data));
		MODM_LOG_INFO << "BitBangI2cMaster: read " << (ok ? "ok" : "failed") << ", the steps:" << modm::endl;
		ring = recorder.ring(BitBang::traceInstance());
		printI2cTrace<BitBang>(modm::log::info, *ring);
		ring->resume();

		modm::delay(1s);
	}
}
