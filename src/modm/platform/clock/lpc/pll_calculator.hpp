/*
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LPC_PLL_HPP
#define MODM_LPC_PLL_HPP

#include "../generic/common_clock.hpp"

using namespace modm::clock;

namespace modm
{
	namespace lpc
	{

		template<int64_t InputFrequency, int64_t SystemFrequency>
		class
		Lpc11PllSettings
		{
		private:
			static constexpr int64_t InputMin  = MHz10;
			static constexpr int64_t InputMax  = MHz25;
			// Pll Constant Range
			static constexpr int64_t Mmin =   1;
			static constexpr int64_t Mmax =  32;

//------------------------------- PllM -----------------------------------------
			static constexpr int64_t
			checkM(int64_t m)
			{
				return (calculatePllP(m) >= 0);
			}

			static constexpr int64_t
			calculatePllM(int64_t m = Mmin)
			{
				return checkM(m)? m : ((m < Mmax)? calculatePllM(m + 1) : -1);
			}

//------------------------------- PllP -----------------------------------------
			static constexpr int64_t
			pllP(int64_t m)
			{
				// SystemFrequency = InputFrequency * PllM / PllP
				// => PllP = InputFrequency * PllM * / SystemFrequency
				return InputFrequency * m / SystemFrequency;
			}

			static constexpr int64_t
			checkP(int64_t m, int64_t p)
			{
				// SystemFrequency = InputFrequency * PllM / PllP
				return ((p == 1 || p == 2 || p == 4 || p == 8) &&
						(InputFrequency * m / p) == SystemFrequency);
			}

			static constexpr int64_t
			calculatePllP(int64_t m)
			{
				return checkP(m, pllP(m))? pllP(m) : -1;
			}

			// Internal Pll Constants Representation
			static constexpr int64_t _PllM = calculatePllM(Mmin);		// TODO: remove default
			static constexpr int64_t _PllP = calculatePllP(_PllM);
		public:
			// Pll Constants casted to the correct datatype
			static constexpr uint8_t  PllM = (_PllM > 0)? static_cast<uint8_t>(_PllM)  : 0xff;
			static constexpr uint8_t  PllP = (_PllP > 0)? static_cast<uint8_t>(_PllP)  : 0xff;
			// Resulting Frequencies
			static constexpr int64_t SystemClock = InputFrequency * PllM / PllP;
		private:
			// Static Asserts
			// Check Ranges
			static_assert(PllM >= Mmin && PllM <= Mmax, "MSEL is out of range!");
			static_assert(PllP == 1 || PllP == 2 || PllP == 4 || PllP == 8,
					"PSEL is an invalid value (possible values are 1,2,4,8)!");
			// Check that InputFrequency is between 10 and 25 MHz
			static_assert(InputFrequency >= InputMin && InputFrequency <= InputMax,
				"InputFrequency needs to be between 10MHz and 25MHz! "
				"Please consult your LPC11 Reference Manual.");
			// Check if desired clock frequency is met
			static_assert(SystemClock == SystemFrequency,
				"Desired Output Frequency could not be met."
				"Please consult your LPC11 Reference Manual.");
		};
	}
}

#endif	//  MODM_LPC_PLL_HPP
