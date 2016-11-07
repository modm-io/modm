/*
 * Copyright (c) 2015, Christian Menard
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/pwm/pca9685.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock systemClock;

int 
main()
{
	// Set-up the I2C device as master and configure it to a baudrate of 100.000
	I2cMaster::initialize<systemClock, 100000>();

	// Enable interrupts
	sei();

	// Create the PWM controller and set it up to use I2C address 0x50.
	// The controller's address depends on the configuration of pins A0-A5 and
	// is in range of 0x40 to 0x7f
	xpcc::Pca9685<I2cMaster> pwmController = xpcc::Pca9685<I2cMaster>(0x50);


	// Initialize the controller. I use RGB-LEDs with common annode.
	// Therfore the controllers output needs to be inverted.
	RF_CALL_BLOCKING(pwmController.initialize(0, xpcc::pca9685::MODE2_INVRT));

	uint16_t pwm = 0;

	while (true)
	{
		// Fade all LEDS but LED 5 and 11
		RF_CALL_BLOCKING(pwmController.setAllChannels(pwm));
		// LED 5 is always off
		RF_CALL_BLOCKING(pwmController.setChannel(5, 0));
		// LED 11 is always on
		RF_CALL_BLOCKING(pwmController.setChannel(11, 0xfff));

		// Increase brighness with each cycle. It would be better to use a
		// timer instead...
		pwm++;
		if (pwm == 0x1000)
			pwm = 0;
	}
}
