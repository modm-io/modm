// coding: utf-8
/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "mcp2515_can_bit_timings_test.hpp"

#include "../../../architecture/platform/driver/clock/generic/common_clock.hpp"
#include "../mcp2515_bit_timings.hpp"

using namespace xpcc;
using namespace xpcc::clock;

#define TEST_TIMING(clk, bitrate, sjw, prop, ps1, ps2, prescaler) \
	TEST_ASSERT_EQUALS((xpcc::CanBitTimingMcp2515<clk, Can::Bitrate::bitrate>::getSJW()), sjw); \
	TEST_ASSERT_EQUALS((xpcc::CanBitTimingMcp2515<clk, Can::Bitrate::bitrate>::getProp()), prop); \
	TEST_ASSERT_EQUALS((xpcc::CanBitTimingMcp2515<clk, Can::Bitrate::bitrate>::getPS1()), ps1); \
	TEST_ASSERT_EQUALS((xpcc::CanBitTimingMcp2515<clk, Can::Bitrate::bitrate>::getPS2()), ps2); \
	TEST_ASSERT_EQUALS((xpcc::CanBitTimingMcp2515<clk, Can::Bitrate::bitrate>::getPrescaler()), prescaler);

void
Mcp2515CanBitTimingsTest::testPrecalculatedValues()
{
	//          Clock  Baud    SJW Prop PS1 PS2 Prescaler
	TEST_TIMING(MHz8,  kBps10,   1,   1,  4,  2,  100);
	TEST_TIMING(MHz8,  kBps20,   1,   1,  4,  2,   50);
	TEST_TIMING(MHz8,  kBps125,  1,   1,  4,  2,    8);
	TEST_TIMING(MHz8,  kBps250,  1,   1,  4,  2,    4);
	TEST_TIMING(MHz8,  kBps500,  1,   1,  4,  2,    2);
	TEST_TIMING(MHz8,  MBps1,    1,   1,  1,  1,    2);

	//          Clock  Baud    SJW Prop PS1 PS2 Prescaler
	TEST_TIMING(MHz16, kBps10,   1,   3,  8,  4,  100);
	TEST_TIMING(MHz16, kBps20,   1,   3,  8,  4,   50);
	TEST_TIMING(MHz16, kBps125,  1,   3,  8,  4,    8);
	TEST_TIMING(MHz16, kBps250,  1,   3,  8,  4,    4);
	TEST_TIMING(MHz16, kBps500,  1,   3,  8,  4,    2);
	TEST_TIMING(MHz16, MBps1,    1,   1,  4,  2,    2);

	//          Clock  Baud    SJW Prop PS1 PS2 Prescaler
	TEST_TIMING(MHz20, kBps10,   3,   5,  8,  4,  100);
	TEST_TIMING(MHz20, kBps20,   3,   5,  8,  4,   50);
	TEST_TIMING(MHz20, kBps125,  3,   5,  8,  4,    8);
	TEST_TIMING(MHz20, kBps250,  3,   5,  8,  4,    4);
	TEST_TIMING(MHz20, kBps500,  3,   5,  8,  4,    2);
	TEST_TIMING(MHz20, MBps1,    1,   3,  4,  2,    2);
}

