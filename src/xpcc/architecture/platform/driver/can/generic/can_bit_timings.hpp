/*
 * Copyright (c) 2016, Kevin Laeufer
 * Copyright (c) 2016, Antal Szabo
 * Copyright (c) 2016, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_COMMON_CAN_BIT_TIMINGS_HPP
#define XPCC_COMMON_CAN_BIT_TIMINGS_HPP

#include <xpcc/architecture/interface.hpp>

#include "../clock/common_clock.hpp"

namespace xpcc
{

/**
 * CAN Bit Timing
 *
 * Example for CAN bit timing:
 *   CLK on APB1 = 36 MHz
 *   BaudRate = 125 kBPs = 1 / NominalBitTime
 *   NominalBitTime = 8uS = tq + tBS1 + tBS2
 * with:
 *   tBS1 = tq * (TS1[3:0] + 1) = 12 * tq
 *   tBS2 = tq * (TS2[2:0] + 1) = 5 * tq
 *   tq = (BRP[9:0] + 1) * tPCLK
 * where tq refers to the Time quantum
 *   tPCLK = time period of the APB clock = 1 / 36 MHz
 *
 * STM32F1xx   tPCLK = 1 / 36 MHz
 * STM32F20x   tPCLK = 1 / 30 MHz
 * STM32F40x   tPCLK = 1 / 42 MHz
 *
 *
 * @author	Kevin Laeufer
 * @ingroup	can
 */
template<int32_t Clk, int32_t Bitrate>
class CanBitTiming
{
private:
	static constexpr uint16_t calculatePrescaler(uint8_t bs1, uint8_t bs2) {
		return Clk / (Bitrate * (1 + bs1 + bs2));
	}

	struct CanBitTimingConfguration {
		uint8_t bs1;	// 1-16
		uint8_t bs2;	// 1-8
		uint8_t sjw;
		uint16_t prescaler;
	};

	static constexpr uint8_t calcBS1() {
		return (Clk ==  xpcc::clock::MHz8)? 11 :
		       (Clk == xpcc::clock::MHz48)? 11 :
		       (Clk == xpcc::clock::MHz30)? 10 :
		       (Clk == xpcc::clock::MHz36)? 12 :
		       (Clk == xpcc::clock::MHz42)? 14 :
		       (Clk == xpcc::clock::MHz32)? 11 : 0;
	}

	static constexpr uint8_t calcBS2() {
		return (Clk ==  xpcc::clock::MHz8)?  4 :
		       (Clk == xpcc::clock::MHz48)?  4 :
		       (Clk == xpcc::clock::MHz30)?  4 :
		       (Clk == xpcc::clock::MHz36)?  5 :
		       (Clk == xpcc::clock::MHz42)?  6 :
		       (Clk == xpcc::clock::MHz32)?  4 : 0;
	}

	static constexpr CanBitTimingConfguration calculateBestConfig() {
		return { calcBS1(), calcBS2(), 1, calculatePrescaler(calcBS1(), calcBS2()) };
	}

	static constexpr CanBitTimingConfguration BestConfig = calculateBestConfig();

public:
	static constexpr uint8_t getBS1() { return BestConfig.bs1; }
	static constexpr uint8_t getBS2() { return BestConfig.bs2; }
	static constexpr uint8_t getSJW() { return BestConfig.sjw; }
	static constexpr uint8_t getPrescaler() { return BestConfig.prescaler; }

private:
	// check assertions
	static_assert(getBS1() > 0 and getBS2() > 0,
		"Unsupported frequency for Can peripheral. "
		"Only 8MHz, 30 Mhz, 36 MHz, 42 MHz and 48 MHz are supported at the moment.");
	static_assert(getPrescaler() > 0, "CAN bitrate is too high for standard bit timings!");
};

}	// namespace xpcc

#endif // XPCC_COMMON_CAN_BIT_TIMINGS_HPP

