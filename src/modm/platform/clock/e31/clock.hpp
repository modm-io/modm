/*
 * Copyright (c) 2019, Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_E31_CLOCK_HPP
#define MODM_E31_CLOCK_HPP

#include <stdint.h>
#include "common.hpp"

namespace modm
{
	namespace platform
	{
		/**
		 * Clock management
		 *
		 * \ingroup
		 */
		class PRCI
		{
		public:
			enum class
			PllSource
			{
				InternalOscillator,
				ExternalOscillator
			};

			enum class
			PllPrediv : uint8_t
			{
				NoDiv = 0x0,
				Div2  = 0x1,
				Div3  = 0x2,
				Div4  = 0x3
			};

			enum class
			PllPostDiv : uint8_t
			{
				Div2  = 0x1,
				Div4  = 0x2,
				Div8  = 0x3,
			};

		public:
			static void
			enableInternalOscillator(uint8_t divider, uint8_t trim = 0x10);

			static void
			disableInternalOscillator(void);

			static void
			enableExternalOscillator(void);

			static void
			disableExternalOscillator(void);

			static void
			setupPll(PllSource src,
					 bool bypass,
					 PllPrediv pll_r,
					 uint8_t pll_f,
					 PllPostDiv pll_q);

			static void
			disablePll();

			static inline uint32_t
			getCpuFrequency()
			{
				return modm::clock::fcpu;
			}

			static inline uint32_t
			getCpuFrequencykHz()
			{
				return modm::clock::fcpu_kHz;
			}

			static inline uint32_t
			getCpuFrequencyMHz()
			{
				return modm::clock::fcpu_MHz;
			}
		};
	}
}

#endif	//  MODM_E31_CLOCK_HPP
