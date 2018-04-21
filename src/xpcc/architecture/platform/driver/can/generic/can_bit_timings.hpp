// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * Copyright (c) 2018, Christopher Durand
 * Copyright (c) 2018, Raphael Lehmann
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_COMMON_CAN_BIT_TIMINGS_HPP
#define XPCC_COMMON_CAN_BIT_TIMINGS_HPP

#include <xpcc/architecture/interface.hpp>

#include "../../clock/generic/common_clock.hpp"

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
	struct CanBitTimingConfiguration {
		uint8_t bs1;	// 1-16
		uint8_t bs2;	// 1-8
		uint8_t sjw;
		uint16_t prescaler;
		float minError;
	};

	static constexpr uint32_t round_uint32(float f)
	{
		uint32_t f_int = (uint32_t) f;
		if(f - f_int > 0.5)
			return f_int + 1;
		else
			return f_int;
	}

	static constexpr CanBitTimingConfiguration calculateBestConfig()
	{
		constexpr uint8_t minBs1Bs2 = 14;
		constexpr uint8_t maxBs1Bs2 = 20;

		float minError = 10000;
		uint16_t bestPrescaler = 0;
		uint8_t bestBs1Bs2 = 0;

		for(uint8_t bs1Bs2 = minBs1Bs2; bs1Bs2 <= maxBs1Bs2; ++bs1Bs2) {
			float idealPrescaler = float(Clk) / (Bitrate * (1 + bs1Bs2));
			uint32_t intPrescaler = round_uint32(idealPrescaler);
			float error = fabs(1 - intPrescaler/idealPrescaler);
			if(error <= minError) {
				bestPrescaler = intPrescaler;
				minError = error;
				bestBs1Bs2 = bs1Bs2;
			}
		}

		uint8_t bs2 = round_uint32(0.275f * (bestBs1Bs2 + 1));
		uint8_t bs1 = bestBs1Bs2 - bs2;

		return CanBitTimingConfiguration{bs1, bs2, 1, bestPrescaler, minError};
	}

	static constexpr CanBitTimingConfiguration BestConfig = calculateBestConfig();

public:
	static constexpr uint8_t getBS1() { return BestConfig.bs1; }
	static constexpr uint8_t getBS2() { return BestConfig.bs2; }
	static constexpr uint8_t getSJW() { return BestConfig.sjw; }
	static constexpr uint8_t getPrescaler() { return BestConfig.prescaler; }

private:
	// check assertions
	static_assert(getPrescaler() > 0, "CAN bitrate is too high for standard bit timings!");
	static_assert(getPrescaler() < ((1 << 10) - 1), "Prescaler value too large"); // 10 bit prescaler on STM32
};

}	// namespace xpcc

#endif // XPCC_COMMON_CAN_BIT_TIMINGS_HPP

