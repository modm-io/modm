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

#include <modm/platform.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/driver/gpio_sampler.hpp>

#include "gpio_sampler_test.hpp"

#include <modm/debug/logger.hpp>

using namespace modm::platform;
using namespace std::chrono_literals;

void
GpioSamplerTest::testTiming()
{
	using Gpio0 = GpioA0;
	using Gpio1 = GpioA1;
	Gpio0::setOutput(modm::Gpio::Low);
	Gpio1::setOutput(modm::Gpio::High);

	{
		auto r = modm::GpioSampler::Create<Gpio0, Gpio1>(100);
		Gpio0::set();     modm::delay(1ms);
		Gpio0::reset();   modm::delay(1ms);
		Gpio0::set();     modm::delay(1ms);
		Gpio0::reset();   modm::delay(1ms);

		Gpio1::reset();   modm::delay(1ms);
		Gpio1::set();     modm::delay(1ms);
		Gpio1::reset();   modm::delay(1ms);
		Gpio1::set();     modm::delay(1ms);

		TEST_ASSERT_EQUALS(r[0].size(), 5u);
		TEST_ASSERT_EQUALS(r[1].size(), 5u);

		MODM_LOG_INFO << "Channel 0:" << modm::endl;
		for (const auto &t : r[0]) {
			MODM_LOG_INFO << t << modm::endl;
		}
		MODM_LOG_INFO << "Channel 1:" << modm::endl;;
		for (const auto &t : r[1]) {
			MODM_LOG_INFO << t << modm::endl;
		}

		TEST_ASSERT_FALSE(r[0].read(0)); // low
		TEST_ASSERT_TRUE( r[0].read(1)); // high
		TEST_ASSERT_FALSE(r[0].read(2)); // low
		TEST_ASSERT_TRUE( r[0].read(3)); // high
		TEST_ASSERT_FALSE(r[0].read(4)); // low

		TEST_ASSERT_TRUE( r[1].read(0)); // high
		TEST_ASSERT_FALSE(r[1].read(1)); // low
		TEST_ASSERT_TRUE( r[1].read(2)); // high
		TEST_ASSERT_FALSE(r[1].read(3)); // low
		TEST_ASSERT_TRUE( r[1].read(4)); // high

		const int32_t ce = SystemCoreClock / 1000;
		const int32_t cu = ce * 1.1f;

		TEST_ASSERT_EQUALS(r[0].diff(0), 0);
		TEST_ASSERT_EQUALS_RANGE(r[0].diff(1), 0l, ce);
		TEST_ASSERT_EQUALS_RANGE(r[0].diff(2), ce, cu);
		TEST_ASSERT_EQUALS_RANGE(r[0].diff(3), ce, cu);
		TEST_ASSERT_EQUALS_RANGE(r[0].diff(4), ce, cu);

		TEST_ASSERT_EQUALS(r[1].diff(0), 0);
		TEST_ASSERT_EQUALS_RANGE(r[1].diff(1), ce*4, ce*3 + cu);
		TEST_ASSERT_EQUALS_RANGE(r[1].diff(2), ce, cu);
		TEST_ASSERT_EQUALS_RANGE(r[1].diff(3), ce, cu);
		TEST_ASSERT_EQUALS_RANGE(r[1].diff(4), ce, cu);
	}
}

void
GpioSamplerTest::testVerifier()
{

}


