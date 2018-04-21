// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "can_bit_timings_test.hpp"

#include "../driver/clock/generic/common_clock.hpp"
#include "../driver/can/generic/can_bit_timings.hpp"

using namespace xpcc;
using namespace xpcc::clock;

#define TEST_TIMING(clk, bitrate, bs1, bs2, prescaler) \
	TEST_ASSERT_EQUALS((xpcc::CanBitTiming<clk, Can::Bitrate::bitrate>::getBS1()), bs1); \
	TEST_ASSERT_EQUALS((xpcc::CanBitTiming<clk, Can::Bitrate::bitrate>::getBS2()), bs2); \
	TEST_ASSERT_EQUALS((xpcc::CanBitTiming<clk, Can::Bitrate::bitrate>::getPrescaler()), prescaler);

void
CanBitTimingsTest::testPrecalculatedValues()
{
	// this tests some timings, that were calculated by hand
	// and used in xpcc from 2011 through 2015
	//
	// This is the old code:
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//		constexpr uint8_t bs1 = (SystemClock::Can{{ id }} ==  MHz8)? 11 :
	//							(SystemClock::Can{{ id }} == MHz48)? 11 :
	//							(SystemClock::Can{{ id }} == MHz30)? 10 :
	//							(SystemClock::Can{{ id }} == MHz36)? 12 :
	//							(SystemClock::Can{{ id }} == MHz42)? 14 : 0;
	//	constexpr uint8_t bs2 = (SystemClock::Can{{ id }} ==  MHz8)?  4 :
	//							(SystemClock::Can{{ id }} == MHz48)?  4 :
	//							(SystemClock::Can{{ id }} == MHz30)?  4 :
	//							(SystemClock::Can{{ id }} == MHz36)?  5 :
	//							(SystemClock::Can{{ id }} == MHz42)?  6 : 0;
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	TEST_TIMING(MHz8,  kBps125, 11, 4,  4);
	TEST_TIMING(MHz8,  kBps250, 11, 4,  2);
	TEST_TIMING(MHz8,  kBps500, 11, 4,  1);
	// @ 8MHZ MBps1 was not supported (resulted in prescaler = 0)

	TEST_TIMING(MHz48, kBps125, 11, 4, 24);
	TEST_TIMING(MHz48, kBps250, 11, 4, 12);
	TEST_TIMING(MHz48, kBps500, 11, 4,  6);
	TEST_TIMING(MHz48,   MBps1, 11, 4,  3);

	TEST_TIMING(MHz30, kBps125, 14, 5, 12);
	TEST_TIMING(MHz30, kBps250, 14, 5,  6);
	TEST_TIMING(MHz30, kBps500, 14, 5,  3);
	TEST_TIMING(MHz30,   MBps1, 10, 4,  2);

	TEST_TIMING(MHz36, kBps125, 12, 5, 16);
	TEST_TIMING(MHz36, kBps250, 12, 5,  8);
	TEST_TIMING(MHz36, kBps500, 12, 5,  4);
	TEST_TIMING(MHz36,   MBps1, 12, 5,  2);

	TEST_TIMING(MHz42, kBps125, 14, 6, 16);
	TEST_TIMING(MHz42, kBps250, 14, 6,  8);
	TEST_TIMING(MHz42, kBps500, 14, 6,  4);
	TEST_TIMING(MHz42,   MBps1, 14, 6,  2);

	TEST_TIMING(MHz32, kBps125, 11, 4, 16);
	TEST_TIMING(MHz32, kBps250, 11, 4,  8);
	TEST_TIMING(MHz32, kBps500, 11, 4,  4);
	TEST_TIMING(MHz32,   MBps1, 11, 4,  2);

	TEST_TIMING(MHz80,  kBps125, 14, 5, 32);
	TEST_TIMING(MHz80,  kBps250, 14, 5, 16);
	TEST_TIMING(MHz80,  kBps500, 14, 5,  8);
	TEST_TIMING(MHz80,    MBps1, 14, 5,  4);

	TEST_TIMING(MHz180,  kBps125, 14, 5, 72);
	TEST_TIMING(MHz180,  kBps250, 14, 5, 36);
	TEST_TIMING(MHz180,  kBps500, 14, 5, 18);
	TEST_TIMING(MHz180,    MBps1, 14, 5,  9);
}

