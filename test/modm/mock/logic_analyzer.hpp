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

#pragma once

#include <modm/driver/gpio_sampler.hpp>
#include <stdlib.h>
#include <limits.h>
#include <functional>
#include <modm/debug/logger.hpp>

namespace modm_test
{


class LogicAnalyzer
{
public:
	using FailureHandler = std::function<void(const char *state, size_t index, size_t time)>;

	static inline bool
	verify(const char *state_machine,
	       const modm::GpioSampler::Channel &ch,
	       FailureHandler failure = nullptr)
	{
		const auto fn_fail = [&ch](const char *state, size_t index, int32_t time)
		{
			MODM_LOG_ERROR << "failed at '" << *state << "' with '" << ch[index] << "'";
			if (time) { MODM_LOG_ERROR << " at diff " << ch.diff(index) << " '" << time << "'"; }
			MODM_LOG_ERROR << modm::endl;
		};
		if (failure == nullptr) failure = fn_fail;

		const char *state = state_machine;
		const char *repeat = nullptr;
		size_t repeat_count = 0;

		int32_t tmin = 0;
		int32_t tmax = INT_MAX;
		volatile int32_t *time = &tmin;

		size_t sidx = 0;

		while(*state)
		{
			switch(*state)
			{
				case 'l': // low sample
				case 'h': // high sample
				{
					if (int32_t diff = ch.diff(sidx); diff) {
						if (diff < tmin) failure(state, sidx, -tmin);
						if (tmax < diff) failure(state, sidx, tmax);
					}

					int32_t sample = ch[sidx];
					if (*state == 'l') {
						if (0 < sample) failure(state, sidx, 0);
						// MODM_LOG_DEBUG << "l " << sample << modm::endl;
					} else {
						if (sample < 0) failure(state, sidx, 0);
						// MODM_LOG_DEBUG << "h " << sample << modm::endl;
					}
					time = &tmin;
					break;
				}
				case '>': // next sample
					if (sidx >= ch.max()) failure(state, sidx, 0);
					time = &tmax;
					sidx++;
					// MODM_LOG_DEBUG << '>' << modm::endl;
					break;

				case '0'...'9': // timing information
				{
					char *out;
					const uint64_t us = strtoul(state, &out, 10);
					const uint32_t cycles = (SystemCoreClock * us) / 1000000;
					*time = cycles;
					state = out;
					// MODM_LOG_DEBUG << (time == &tmin ? "min " : "max ") << us << "us => " << cycles << modm::endl;
					continue;
				}

				case '{': // start of repeat count
				{
					char *out;
					repeat_count = strtoul(state+1, &out, 10) - 1;
					state = out;
					// MODM_LOG_DEBUG << '{' << repeat_count << '}' << modm::endl;
					continue;
				}
				case '}': // end of repeat count
					break;

				case '(': // start of repeat group
					repeat = state;
					// MODM_LOG_DEBUG << '(' << modm::endl;
					break;

				case ')': // end of repeat group
					if (repeat_count > 0) {
						repeat_count--;
						state = repeat;
						// MODM_LOG_DEBUG << ")<-" << repeat_count << modm::endl;
						continue;
					}
					break;

				default: // just continue
					break;
			}
			state++;
		}
		return true;
	}
};

} // namespace modm::platform
