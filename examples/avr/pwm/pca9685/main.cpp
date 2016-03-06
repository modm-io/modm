/*
 * Copyright (c) 2015, Christian Menard
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
