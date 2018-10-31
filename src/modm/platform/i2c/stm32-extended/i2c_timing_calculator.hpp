/*
 * Copyright (c) 2018, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_STM32_I2C_TIMING_CALCULATOR_HPP
#define MODM_STM32_I2C_TIMING_CALCULATOR_HPP

#include <tuple>
#include <optional>
#include <array>
#include <cmath>

namespace modm
{

namespace platform
{

struct I2cMasterTimings
{
	uint8_t prescaler;
	uint8_t sdaDel;
	uint8_t sclDel;
	uint8_t sclLow;
	uint8_t sclHigh;
};

struct I2cParameters
{
	uint32_t peripheralClock;
	uint32_t targetSpeed;
	uint16_t tolerance;
	uint8_t digitalFilterLength;
	bool enableAnalogFilter;
	float riseTime;
	float fallTime;
};

/**
 * STM32 extended I2C timing calculator
 *
 * @author		Christopher Durand
 * @ingroup		modm_platform_i2c
 */
class I2cTimingCalculator
{
public:
	constexpr I2cTimingCalculator(const I2cParameters& parameters) : params{parameters} {}

	constexpr std::optional<I2cMasterTimings>
	calculateTimings() const
	{
		uint16_t prescalerMask{findValidPrescalers()};
		if(prescalerMask == 0) {
			return std::nullopt;
		}

		bool found = false;
		float lastError = 1e10;
		uint8_t bestSclLow = 0;
		uint8_t bestSclHigh = 0;
		uint8_t bestPrescaler = 16;

		for(int prescaler = 15; prescaler >= 0; --prescaler)
		{
			if(not (prescalerMask & (1 << prescaler))) {
				continue;
			}

			for(uint16_t sclLow = 0; sclLow <= 255; ++sclLow) {
				for(uint16_t sclHigh = 0; sclHigh <= 255; ++sclHigh) {
					std::optional<float> speed = calculateSpeed(prescaler, sclLow, sclHigh);
					if(speed) {
						auto error = std::fabs((speed.value() / params.targetSpeed) - 1.0f);
						// modm::Tolerance value is in unit [1/1000]
						if((error * 1000) < params.tolerance && error <= lastError && prescaler <= bestPrescaler) {
							lastError = error;
							bestPrescaler = prescaler;
							bestSclLow = sclLow;
							bestSclHigh = sclHigh;
							found = true;
						}
					}
				}
			}
		}

		if(found) {
			I2cMasterTimings timings = {};
			timings.prescaler = bestPrescaler;

			auto [sdaDel, sclDel] = findHoldTimeSettings(bestPrescaler).value();
			timings.sdaDel = sdaDel;
			timings.sclDel = sclDel;

			timings.sclLow = bestSclLow;
			timings.sclHigh = bestSclHigh;

			return timings;
		}

		return std::nullopt;
	}

	static constexpr uint32_t
	timingsToRegisterValue(const I2cMasterTimings& timings)
	{
		uint32_t value = timings.sclLow;
		value |= timings.sclHigh << 8;
		value |= (timings.sdaDel & 0b1111) << 16;
		value |= (timings.sclDel & 0b1111) << 20;
		value |= (timings.prescaler & 0b1111) << 28;
		return value;
	}

private:
	I2cParameters params;

	struct ModeConstants
	{
		// index 0: standard	100kHz
		// index 1: fast		400kHz
		// index 2: fast+		1000kHz

		static constexpr std::array tFallMax	= {
			300.0e-9f,
			300.0e-9f,
			120.0e-9f
		};

		static constexpr std::array tRiseMax	= {
			1000.0e-9f,
			300.0e-9f,
			120.0e-9f
		};

		static constexpr std::array tHdDatMax = {
			3450.0e-9f,
			900.0e-9f,
			450.0e-9f
		};

		static constexpr std::array tSuDatMin = {
			250.0e-9f,
			100.0e-9f,
			50.0e-9f
		};

		static constexpr std::array tLowMin	= {
			4.7e-6f,
			1.3e-6f,
			0.5e-6f
		};

		static constexpr std::array tHighMin	= {
			4.0e-6f,
			0.6e-6f,
			0.26e-6f
		};
	};

	static constexpr auto FastThreshold = 125'000.f;
	static constexpr auto FastPlusThreshold = 500'000.f;

	static constexpr auto AnalogFilterDelayMin = 50.0e-9f;
	static constexpr auto AnalogFilterDelayMax = 260.0e-9f;

	constexpr uint8_t
	modeIndex() const
	{
		if(params.targetSpeed < FastThreshold) {
			return 0;
		} else if(params.targetSpeed < FastPlusThreshold) {
			return 1;
		} else {
			return 2;
		}
	}

	/// Calculate hold time settings for a specific prescaler
	constexpr std::optional<std::tuple<uint16_t, uint16_t>>
	findHoldTimeSettings(uint8_t prescaler) const
	{
		auto sdaDelMinTime = params.fallTime - AnalogFilterDelayMin
			- (float(params.digitalFilterLength + 3) / params.peripheralClock);

		sdaDelMinTime = std::max(0.0f, sdaDelMinTime);

		auto sdaDelMaxTime = ModeConstants::tHdDatMax[modeIndex()];
		sdaDelMaxTime -= params.riseTime;
		sdaDelMaxTime -= (float(params.digitalFilterLength + 4) / params.peripheralClock);
		sdaDelMaxTime -= AnalogFilterDelayMax;

		sdaDelMaxTime = std::max(0.0f, sdaDelMaxTime);

		auto sclDelMinTime = params.riseTime + ModeConstants::tSuDatMin[modeIndex()];

		for(uint8_t sdaDel = 0; sdaDel <= 15; ++sdaDel) {
			auto sdaDelTime = float(sdaDel) * (prescaler + 1) / params.peripheralClock;

			if(sdaDelTime < sdaDelMinTime || sdaDelTime > sdaDelMaxTime) {
				continue;
			}

			for(uint8_t sclDel = 0; sclDel <= 15; ++sclDel) {
				auto sclDelTime = float(sclDel + 1) * (prescaler + 1) / params.peripheralClock;

				if(sclDelTime >= sclDelMinTime) {
					return std::tuple{sdaDel, sclDel};
				}
			}
		}

		return std::nullopt;
	}

	/// Find prescaler values for which valid hold times can be achieved
	// \return Bit mask of valid prescaler config values (0 to 15)
	constexpr uint16_t
	findValidPrescalers() const
	{
		uint16_t prescalerMask = 0;

		for(uint8_t prescaler = 0; prescaler <= 15; ++prescaler) {
			if(findHoldTimeSettings(prescaler)) {
				prescalerMask |= (1 << prescaler);
			}
		}

		return prescalerMask;
	}

	constexpr std::optional<float>
	calculateSpeed(uint8_t prescaler, uint16_t sclLow, uint16_t sclHigh) const
	{
		auto clockPeriod = float(prescaler + 1) / params.peripheralClock;

		auto filterDelay = (params.enableAnalogFilter ? AnalogFilterDelayMin : 0)
			+ float(params.digitalFilterLength) / params.peripheralClock;

		auto syncTime = filterDelay + (2.0f / params.peripheralClock);

		auto sclLowTime = (sclLow + 1) * clockPeriod + syncTime;
		auto sclHighTime = (sclHigh + 1) * clockPeriod + syncTime;

		auto sclTime = sclLowTime + sclHighTime + params.riseTime + params.fallTime;
		auto speed = 1.0f / sclTime;

		if(sclLowTime >= ModeConstants::tLowMin[modeIndex()]
			&& sclHighTime >= ModeConstants::tHighMin[modeIndex()]
			&& ((1.0f / params.peripheralClock) < ((sclLowTime - filterDelay) / 4))
			&& ((1.0f / params.peripheralClock) < sclHighTime)) {

			return speed;
		} else {
			return std::nullopt;
		}
	}
};

} // namespace platform

} // namespace modm

#endif // MODM_STM32_I2C_TIMING_CALCULATOR_HPP
