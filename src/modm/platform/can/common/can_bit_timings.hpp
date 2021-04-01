/*
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2016, Kevin Läufer
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COMMON_CAN_BIT_TIMINGS_HPP
#define MODM_COMMON_CAN_BIT_TIMINGS_HPP

#include <modm/architecture/interface/clock.hpp>
#include <modm/math/units.hpp>
#include <modm/math/utils/misc.hpp>
#include <cmath>

namespace modm
{


struct CanBitTimingConfiguration
{
	uint8_t bs1;
	uint8_t bs2;
	uint8_t sjw;
	uint16_t prescaler;
	float error;
};

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
 * @ingroup	modm_platform_can_common
 */
template<int32_t Clk, int32_t Bitrate, uint8_t prescaler_width = 10, uint8_t bs1_width = 4, uint8_t bs2_width = 3, uint8_t sjw_width = 2>
class CanBitTiming
{
private:
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

		float minError = 10'000.0;
		uint16_t bestPrescaler = 0;
		uint8_t bestBs1Bs2 = 0;

		for(uint8_t bs1Bs2 = minBs1Bs2; bs1Bs2 <= maxBs1Bs2; ++bs1Bs2) {
			float idealPrescaler = float(Clk) / (Bitrate * (1 + bs1Bs2));
			uint32_t intPrescaler = round_uint32(idealPrescaler);

			if(intPrescaler < ((1 << prescaler_width) - 1)) {
				float error = constexpr_fabs(1 - intPrescaler/idealPrescaler);
				if(error <= minError) {
					bestPrescaler = intPrescaler;
					minError = error;
					bestBs1Bs2 = bs1Bs2;
				}
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

	static constexpr CanBitTimingConfiguration
	getBitTimings()
	{
		return BestConfig;
	}

	template<percent_t tolerance>
	static constexpr void assertBitrateInTolerance()
	{
		static_assert(pct2f(tolerance) >= BestConfig.error,
			"The closest available bitrate exceeds the specified maximum tolerance!");
	}

private:
	// check assertions
	static_assert(getPrescaler() > 0, "CAN bitrate is too high for standard bit timings!");
	static_assert(getPrescaler() <= (1 << prescaler_width), "Prescaler value too large");
	static_assert(getBS1() <= (1 << bs1_width), "BS1 value too large");
	static_assert(getBS2() <= (1 << bs2_width), "BS2 value too large");
	static_assert(getSJW() <= (1 << sjw_width), "SJW value too large");
};

}	// namespace modm

#endif // MODM_COMMON_CAN_BIT_TIMINGS_HPP

