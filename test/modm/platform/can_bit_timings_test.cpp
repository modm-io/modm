/*
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2016, Kevin Läufer
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "can_bit_timings_test.hpp"

#include <modm/architecture/interface/clock.hpp>
#include <modm/architecture/interface/can.hpp>
#include <modm/platform/can/can_bit_timings.hpp>

using namespace modm;
using namespace modm::literals;

#define TEST_TIMING(clk, bitrate, bs1, bs2, prescaler) \
	TEST_ASSERT_EQUALS((modm::CanBitTiming<clk, Can::Bitrate::bitrate>::getBS1()), bs1); \
	TEST_ASSERT_EQUALS((modm::CanBitTiming<clk, Can::Bitrate::bitrate>::getBS2()), bs2); \
	TEST_ASSERT_EQUALS((modm::CanBitTiming<clk, Can::Bitrate::bitrate>::getPrescaler()), prescaler);

void
CanBitTimingsTest::testPrecalculatedValues()
{
	// this tests some timings, that were calculated by hand
	// and used in modm from 2011 through 2015
	//
	// This is the old code:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//		constexpr uint8_t bs1 = (SystemClock::Can{{ id }} ==  8_MHz)? 11 :
	//							(SystemClock::Can{{ id }} == 48_MHz)? 11 :
	//							(SystemClock::Can{{ id }} == 30_MHz)? 10 :
	//							(SystemClock::Can{{ id }} == 36_MHz)? 12 :
	//							(SystemClock::Can{{ id }} == 42_MHz)? 14 : 0;
	//	constexpr uint8_t bs2 = (SystemClock::Can{{ id }} ==  8_MHz)?  4 :
	//							(SystemClock::Can{{ id }} == 48_MHz)?  4 :
	//							(SystemClock::Can{{ id }} == 30_MHz)?  4 :
	//							(SystemClock::Can{{ id }} == 36_MHz)?  5 :
	//							(SystemClock::Can{{ id }} == 42_MHz)?  6 : 0;
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TEST_TIMING(8_MHz,  kBps125, 11, 4,  4);
	TEST_TIMING(8_MHz,  kBps250, 11, 4,  2);
	TEST_TIMING(8_MHz,  kBps500, 11, 4,  1);
	// @ 8MHZ MBps1 was not supported (resulted in prescaler = 0)

	TEST_TIMING(48_MHz, kBps125, 11, 4, 24);
	TEST_TIMING(48_MHz, kBps250, 11, 4, 12);
	TEST_TIMING(48_MHz, kBps500, 11, 4,  6);
	TEST_TIMING(48_MHz,   MBps1, 11, 4,  3);

	TEST_TIMING(30_MHz, kBps125, 14, 5, 12);
	TEST_TIMING(30_MHz, kBps250, 14, 5,  6);
	TEST_TIMING(30_MHz, kBps500, 14, 5,  3);
	TEST_TIMING(30_MHz,   MBps1, 10, 4,  2);

	TEST_TIMING(36_MHz, kBps125, 12, 5, 16);
	TEST_TIMING(36_MHz, kBps250, 12, 5,  8);
	TEST_TIMING(36_MHz, kBps500, 12, 5,  4);
	TEST_TIMING(36_MHz,   MBps1, 12, 5,  2);

	TEST_TIMING(42_MHz, kBps125, 14, 6, 16);
	TEST_TIMING(42_MHz, kBps250, 14, 6,  8);
	TEST_TIMING(42_MHz, kBps500, 14, 6,  4);
	TEST_TIMING(42_MHz,   MBps1, 14, 6,  2);

	TEST_TIMING(32_MHz, kBps125, 11, 4, 16);
	TEST_TIMING(32_MHz, kBps250, 11, 4,  8);
	TEST_TIMING(32_MHz, kBps500, 11, 4,  4);
	TEST_TIMING(32_MHz,   MBps1, 11, 4,  2);

	TEST_TIMING(80_MHz,  kBps125, 14, 5, 32);
	TEST_TIMING(80_MHz,  kBps250, 14, 5, 16);
	TEST_TIMING(80_MHz,  kBps500, 14, 5,  8);
	TEST_TIMING(80_MHz,    MBps1, 14, 5,  4);

	TEST_TIMING(180_MHz,  kBps125, 14, 5, 72);
	TEST_TIMING(180_MHz,  kBps250, 14, 5, 36);
	TEST_TIMING(180_MHz,  kBps500, 14, 5, 18);
	TEST_TIMING(180_MHz,    MBps1, 14, 5,  9);
}

