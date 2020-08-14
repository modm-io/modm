/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/display/is31fl3733.hpp>

using namespace Board;

using PinReset = GpioC8;
using PinSda = GpioA11;
using PinScl = GpioA12;

using I2cInstance = BitBangI2cMaster<PinScl, PinSda>;
static modm::Is31fl3733<I2cInstance> driver(modm::is31fl3733::addr());

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedD13::setOutput(modm::Gpio::Low);

	MODM_LOG_ERROR   << "error"   << modm::endl;

	// Reset the I2C interface of the chip
	PinReset::setOutput(modm::Gpio::High);
	modm::delay(20ms);
	PinReset::setOutput(modm::Gpio::Low);

	I2cInstance::initialize<SystemClock, 400_kHz>();
	I2cInstance::connect<PinScl::BitBang, PinSda::BitBang>();

	RF_CALL_BLOCKING(driver.reset());
	RF_CALL_BLOCKING(driver.clearSoftwareShutdown());
	RF_CALL_BLOCKING(driver.setGlobalCurrent(1));

	driver.enableAll();
	RF_CALL_BLOCKING(driver.writeOnOff());

	uint8_t pwm{0};
	while (true)
	{
		for (uint8_t y=0, pi=pwm; y<16; y++) {
			for (uint8_t x=0; x<12; x++) {
				driver.setPwm(x, y, pi++);
			}
		}
		RF_CALL_BLOCKING(driver.writePwm());
		modm::delay(10ms);
		pwm++;
	}

	return 0;
}
