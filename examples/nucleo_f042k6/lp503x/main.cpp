/*
 * Copyright (c) 2020, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/pwm/lp503x.hpp>

using namespace Board;
using namespace std::chrono_literals;

/*
 * Example to demonstrate LP503x driver
 * It assumes an LP5036 is connected to the following pins:
 *   A10 SDA
 *   A11 SCL
 *   A7  Enable
 */

using I2cSda	= GpioA10;
using I2cScl	= GpioA11;
using LedEnable	= GpioA7;

constexpr uint8_t i2cAddress = 0b011'0000;

int
main()
{
	Board::initialize();
	LedEnable::setOutput();

	MODM_LOG_INFO << "LP5036 demo" << modm::endl;

	I2cMaster1::connect<I2cSda::Sda, I2cScl::Scl>();
	I2cMaster1::initialize<SystemClock, 400_kBd>();

	modm::Lp5036<I2cMaster1> leds{i2cAddress};
	LedEnable::set();

	// Initialize and enable chip
	RF_CALL_BLOCKING(leds.initialize());
	RF_CALL_BLOCKING(leds.enable());

	// Turn on all leds with increasing brightness
	for(uint8_t channel = 0; channel < 36; ++channel) {
		const uint8_t brightness = (channel + 1) * 7;
		RF_CALL_BLOCKING(leds.setChannelBrightness(channel, brightness));
	}

	modm::delay(1s);

	// Configure outputs 0-5 (rgb led 0-1) in bank mode
	using LedBankMode = modm::lp503x::LedBankMode;
	const auto bankLeds = LedBankMode::Led0 | LedBankMode::Led1;
	RF_CALL_BLOCKING(leds.setBankModeEnabled(bankLeds));

	// Set bank leds to full brightness
	RF_CALL_BLOCKING(leds.setBankABrightness(255));
	RF_CALL_BLOCKING(leds.setBankBBrightness(255));
	RF_CALL_BLOCKING(leds.setBankCBrightness(255));

	// Blink leds in bank mode
	while(true) {
		RF_CALL_BLOCKING(leds.setBankBrightness(255));
		modm::delay(0.5s);
		RF_CALL_BLOCKING(leds.setBankBrightness(0));
		modm::delay(0.5s);
	}
}
