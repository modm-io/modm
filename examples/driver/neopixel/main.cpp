/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2023, Thomas Rush
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

using namespace Board;

/*
 * Output a color (GRB) to the Neopixel
 *
 * 'Neopixel' is Adafruit's name for what is essentially a WS2812 RGB LED.
 * The timing, which runs at 800kHz, is basically a long high pulse followed
 * by a short low pulse for a '1' bit, or a short high pulse followed by a
 * long low pulse for a '0' bit. The idle state of the signal is low. The
 * data is 24 bits long (three 8-bit color values sent out in green-red-blue
 * order, MSB first).
 *
 * The long and short pulse times chosen here are nominally 875ns and 375ns,
 * respectively, totalling 1.25us per bit. Using modm::delay_ns() with these
 * values means that the actual times will be slightly longer because of the
 * setup times for each bit, but the cycle time can be longer (within limits)
 * without affecting the operation. (See
 * https://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/
 * for more information about the signal timing for these devices.)
 *
 * This function is not intended to be the best solution as a driver for the
 * WS2812, just an example of a simple application of the modm library.
 */

void
setPixel(uint32_t data)
{
	constexpr uint32_t neoPeriod = 1250;
	constexpr uint32_t neoOneHi  = 875;
	constexpr uint32_t neoZeroHi = 375;

	uint32_t bitMask = (1UL << 23);

	{
		modm::atomic::Lock lock;

		do
		{
			uint32_t hiTime = data & bitMask ? neoOneHi : neoZeroHi;
			Neopixel::toggle(); // toggle high
			modm::delay_ns(hiTime);
			Neopixel::toggle(); // toggle low
			modm::delay_ns(neoPeriod - hiTime);
			bitMask >>= 1;
		}
		while (bitMask);
	}
}

int
main()
{
	initialize();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter(0);

	const char *log_str[] = { "green ", "red ", "blue ", "loop: " };

	while (true)
	{
		// color values are reduced to decrease the brightness of the LED
		// (full intensity can be uncomfortable for the eyes!)
		for (uint32_t i = 0, color = 0x110000; i < 4; i++, color >>= 8) {
			modm::delay(1s);
			setPixel(color);
			MODM_LOG_INFO << log_str[i];
		}
		MODM_LOG_INFO << counter++ << modm::endl;
	}

	return 0;
}

