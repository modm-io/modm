/*
 * Copyright (c) 2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "spi_bitbang_test.hpp"
#include <modm/platform/spi/bitbang_spi_master.hpp>
#include <modm/driver/gpio_sampler.hpp>
#include <modm/board.hpp>

#include <modm-test/mock/logic_analyzer.hpp>

using namespace modm::platform;



void
SpiBitbangTest::testSpiMaster()
{
	using Sck = GpioA0;
	using Mosi = GpioA1;
	using SpiMaster = BitBangSpiMaster<Sck, Mosi>;
	SpiMaster::connect<Sck::BitBang, Mosi::BitBang>();
	SpiMaster::initialize<Board::SystemClock, 10'000>();

	const auto Start = modm::GpioSampler::Create<Sck, Mosi>;

	{
		auto r = Start(50);
		SpiMaster::transferBlocking(0);
		TEST_ASSERT_EQUALS(r[1].size(), 1u);
		TEST_ASSERT_EQUALS(r[0].size(), 17u);

		modm_test::LogicAnalyzer::verify("l", r[1]);                    // MOSI
		modm_test::LogicAnalyzer::verify("l {8}( > h 45>55 l )", r[0]); // SCK

		r[0].dump();
		r[1].dump();

		r.restart();
		SpiMaster::transferBlocking(0b1010'1010);
		TEST_ASSERT_EQUALS(r[1].size(), 9u);
		TEST_ASSERT_EQUALS(r[0].size(), 17u);

		modm_test::LogicAnalyzer::verify("l {4}( > h 95>110 l ) ", r[1]); // MOSI
		modm_test::LogicAnalyzer::verify("l {8}( > h 45>55 l )", r[0]);   // SCK

		r[1].dump();

		r.restart();
		SpiMaster::transferBlocking(0b0001'1110);
		TEST_ASSERT_EQUALS(r[1].size(), 3u);
		TEST_ASSERT_EQUALS(r[0].size(), 17u);

		modm_test::LogicAnalyzer::verify("l 295> h >420 l ", r[1]);     // MOSI
		modm_test::LogicAnalyzer::verify("l {8}( > h 45>55 l )", r[0]); // SCK

		r[1].dump();

		r.restart();
		SpiMaster::setDataOrder(SpiMaster::DataOrder::LsbFirst);
		SpiMaster::transferBlocking(0b0001'1110);
		TEST_ASSERT_EQUALS(r[1].size(), 3u);
		TEST_ASSERT_EQUALS(r[0].size(), 17u);

		r[1].dump();

		modm_test::LogicAnalyzer::verify("l 95> h >420 l ", r[1]);     // MOSI
		modm_test::LogicAnalyzer::verify("l {8}( > h 45>55 l )", r[0]); // SCK
	}
}
